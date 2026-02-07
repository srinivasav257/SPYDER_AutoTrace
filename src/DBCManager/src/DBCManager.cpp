/**
 * @file DBCManager.cpp
 * @brief Implementation of DBC Database Manager with background loading.
 */

#include "DBCManager.h"
#include <QSettings>
#include <QFileInfo>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QDebug>

namespace DBCManager {

//=============================================================================
// DBCLoadWorker
//=============================================================================

void DBCLoadWorker::process(int channelIndex, const QString& filePath)
{
    emit progress(channelIndex, QString("Parsing DBC: %1").arg(QFileInfo(filePath).fileName()));

    DBCParser parser;
    auto db = std::make_shared<DBCDatabase>(parser.parseFile(filePath));

    QString errorMsg;
    if (parser.hasErrors()) {
        QStringList msgs;
        for (const auto& err : parser.errors())
            msgs.append(QString("Line %1: %2").arg(err.line).arg(err.message));
        errorMsg = msgs.join("\n");
    }

    if (db->isEmpty() && !parser.hasErrors()) {
        errorMsg = "DBC file contains no messages";
    }

    emit progress(channelIndex, db->isEmpty()
                  ? "Parsing failed"
                  : QString("Parsed %1 messages, %2 signals")
                        .arg(db->messages.size())
                        .arg(db->totalSignalCount()));

    emit finished(channelIndex, db, errorMsg);
}

//=============================================================================
// DBCDatabaseManager
//=============================================================================

DBCDatabaseManager& DBCDatabaseManager::instance()
{
    static DBCDatabaseManager inst;
    return inst;
}

DBCDatabaseManager::DBCDatabaseManager()
{
    // Create background worker thread
    m_workerThread = new QThread(this);
    m_worker = new DBCLoadWorker;  // no parent — will be moved to thread
    m_worker->moveToThread(m_workerThread);

    // Connect worker signals
    connect(m_worker, &DBCLoadWorker::finished,
            this, &DBCDatabaseManager::onWorkerFinished, Qt::QueuedConnection);
    connect(m_worker, &DBCLoadWorker::progress,
            this, &DBCDatabaseManager::loadProgress, Qt::QueuedConnection);

    m_workerThread->start();
}

DBCDatabaseManager::~DBCDatabaseManager()
{
    m_workerThread->quit();
    m_workerThread->wait(3000);
    delete m_worker;
}

// ---------------------------------------------------------------------------
// Loading
// ---------------------------------------------------------------------------

void DBCDatabaseManager::loadDBCFile(int channelIndex, const QString& filePath)
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return;

    // Validate file exists
    if (!QFileInfo::exists(filePath)) {
        emit loadFinished(channelIndex, false, "File not found: " + filePath);
        return;
    }

    {
        QMutexLocker lock(&m_mutex);
        m_channels[channelIndex].filePath = filePath;
        m_channels[channelIndex].loading = true;
    }

    emit loadStarted(channelIndex, filePath);

    // Invoke worker on background thread
    QMetaObject::invokeMethod(m_worker, "process", Qt::QueuedConnection,
                              Q_ARG(int, channelIndex),
                              Q_ARG(QString, filePath));
}

void DBCDatabaseManager::unloadDBC(int channelIndex)
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return;

    {
        QMutexLocker lock(&m_mutex);
        m_channels[channelIndex].database.reset();
        m_channels[channelIndex].filePath.clear();
        m_channels[channelIndex].loading = false;
    }

    emit databaseUnloaded(channelIndex);
    emit messageListChanged(channelIndex);
}

bool DBCDatabaseManager::isLoaded(int channelIndex) const
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return false;
    QMutexLocker lock(&m_mutex);
    return m_channels[channelIndex].database != nullptr
        && !m_channels[channelIndex].database->isEmpty();
}

bool DBCDatabaseManager::isLoading(int channelIndex) const
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return false;
    QMutexLocker lock(&m_mutex);
    return m_channels[channelIndex].loading;
}

// ---------------------------------------------------------------------------
// Database Access
// ---------------------------------------------------------------------------

std::shared_ptr<const DBCDatabase> DBCDatabaseManager::database(int channelIndex) const
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return nullptr;
    QMutexLocker lock(&m_mutex);
    return m_channels[channelIndex].database;
}

QString DBCDatabaseManager::dbcFilePath(int channelIndex) const
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return {};
    QMutexLocker lock(&m_mutex);
    return m_channels[channelIndex].filePath;
}

// ---------------------------------------------------------------------------
// Encode / Decode
// ---------------------------------------------------------------------------

QMap<QString, double> DBCDatabaseManager::decode(int channelIndex, uint32_t canId,
                                                   const uint8_t* data, int dataLength) const
{
    auto db = database(channelIndex);
    if (!db)
        return {};
    const auto* msg = db->messageById(canId);
    if (!msg)
        return {};
    return msg->decodeAll(data, dataLength);
}

bool DBCDatabaseManager::encode(int channelIndex, uint32_t canId,
                                 const QMap<QString, double>& signalValues,
                                 uint8_t* data, int dataLength) const
{
    auto db = database(channelIndex);
    if (!db)
        return false;
    const auto* msg = db->messageById(canId);
    if (!msg)
        return false;
    msg->encodeAll(signalValues, data, dataLength);
    return true;
}

// ---------------------------------------------------------------------------
// UI Helpers
// ---------------------------------------------------------------------------

QStringList DBCDatabaseManager::messageDisplayList(int channelIndex) const
{
    auto db = database(channelIndex);
    if (!db)
        return {};
    return db->messageDisplayList();
}

QStringList DBCDatabaseManager::messageNames(int channelIndex) const
{
    auto db = database(channelIndex);
    if (!db)
        return {};
    return db->messageNames();
}

QStringList DBCDatabaseManager::signalNames(int channelIndex, uint32_t canId) const
{
    auto db = database(channelIndex);
    if (!db)
        return {};
    const auto* msg = db->messageById(canId);
    if (!msg)
        return {};
    return msg->signalNames();
}

uint32_t DBCDatabaseManager::resolveMessageId(int channelIndex, const QString& displayString) const
{
    // Display string format: "0xNNN - MessageName"
    static QRegularExpression re(R"(0x([0-9A-Fa-f]+))");
    auto match = re.match(displayString);
    if (match.hasMatch()) {
        bool ok;
        uint32_t id = match.captured(1).toUInt(&ok, 16);
        if (ok)
            return id;
    }

    // Try name lookup
    auto db = database(channelIndex);
    if (db) {
        const auto* msg = db->messageByName(displayString);
        if (msg)
            return msg->id;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

void DBCDatabaseManager::savePaths()
{
    QSettings s;
    s.beginGroup("DBCManager");
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        QMutexLocker lock(&m_mutex);
        s.setValue(QString("Channel%1/dbcFilePath").arg(i), m_channels[i].filePath);
    }
    s.endGroup();
}

void DBCDatabaseManager::loadSavedPaths()
{
    QSettings s;
    s.beginGroup("DBCManager");
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        QString path = s.value(QString("Channel%1/dbcFilePath").arg(i)).toString();
        if (!path.isEmpty() && QFileInfo::exists(path)) {
            qInfo() << "[DBCManager] Auto-loading DBC for channel" << i << ":" << path;
            loadDBCFile(i, path);
        }
    }
    s.endGroup();
}

// ---------------------------------------------------------------------------
// Worker callback
// ---------------------------------------------------------------------------

void DBCDatabaseManager::onWorkerFinished(int channelIndex,
                                            std::shared_ptr<DBCDatabase> database,
                                            const QString& errorMsg)
{
    if (channelIndex < 0 || channelIndex >= MAX_CHANNELS)
        return;

    bool success = database && !database->isEmpty();

    {
        QMutexLocker lock(&m_mutex);
        if (success) {
            m_channels[channelIndex].database = database;
        }
        m_channels[channelIndex].loading = false;
    }

    if (success) {
        qInfo() << "[DBCManager] Channel" << channelIndex << "loaded:"
                << database->messages.size() << "messages,"
                << database->totalSignalCount() << "signals";

        // Persist the path on successful load
        savePaths();
    } else {
        qWarning() << "[DBCManager] Channel" << channelIndex << "load failed:" << errorMsg;
    }

    emit loadFinished(channelIndex, success, errorMsg);
    if (success)
        emit messageListChanged(channelIndex);
}

} // namespace DBCManager
