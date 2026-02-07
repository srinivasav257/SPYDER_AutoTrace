#pragma once
/**
 * @file DBCManager.h
 * @brief DBC Database Manager — per-channel DBC management with background parsing.
 *
 * This is the main public API for the DBCManager library.
 *
 * Features:
 * - Load and parse DBC files in a background thread
 * - Associate DBC databases with specific CAN channels
 * - Persist DBC file paths per channel via QSettings
 * - Auto-load saved DBC files on startup
 * - Encode/decode CAN messages using signal definitions
 * - Provide message lists for UI combo boxes
 */

#include "DBCParser.h"
#include <QObject>
#include <QMutex>
#include <QThread>
#include <array>
#include <memory>

namespace DBCManager {

//=============================================================================
// DBCLoadWorker — Background parsing thread
//=============================================================================

/**
 * @brief Worker object for background DBC file parsing.
 * Runs on a dedicated QThread to avoid blocking the UI.
 */
class DBCLoadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DBCLoadWorker(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    /**
     * @brief Parse a DBC file (called on worker thread)
     * @param channelIndex CAN channel index (0 or 1)
     * @param filePath DBC file path
     */
    void process(int channelIndex, const QString& filePath);

signals:
    /**
     * @brief Emitted when parsing is complete
     * @param channelIndex CAN channel index
     * @param database Parsed database (shared pointer)
     * @param errorMsg Empty string on success, error message on failure
     */
    void finished(int channelIndex, std::shared_ptr<DBCDatabase> database, const QString& errorMsg);

    /**
     * @brief Progress indication
     */
    void progress(int channelIndex, const QString& status);
};

//=============================================================================
// DBCDatabaseManager — Singleton manager
//=============================================================================

/**
 * @brief Central manager for DBC databases, one per CAN channel.
 *
 * Usage:
 * @code
 * auto& mgr = DBCDatabaseManager::instance();
 * 
 * // Load DBC for CAN channel 0 (background)
 * mgr.loadDBCFile(0, "/path/to/vehicle.dbc");
 * 
 * // After loadFinished signal:
 * auto* db = mgr.database(0);
 * auto* msg = db->messageById(0x7E0);
 * auto values = msg->decodeAll(rawData, 8);
 * 
 * // Get message list for combo box
 * QStringList msgs = mgr.messageDisplayList(0);
 * @endcode
 */
class DBCDatabaseManager : public QObject
{
    Q_OBJECT

public:
    static constexpr int MAX_CHANNELS = 2;

    static DBCDatabaseManager& instance();

    // === DBC File Loading ===

    /**
     * @brief Load a DBC file for a specific CAN channel (background thread)
     * @param channelIndex 0 or 1
     * @param filePath Path to the .dbc file
     */
    void loadDBCFile(int channelIndex, const QString& filePath);

    /**
     * @brief Unload DBC for a specific channel
     */
    void unloadDBC(int channelIndex);

    /**
     * @brief Check if a channel has a loaded DBC database
     */
    bool isLoaded(int channelIndex) const;

    /**
     * @brief Check if a channel is currently loading
     */
    bool isLoading(int channelIndex) const;

    // === Database Access ===

    /**
     * @brief Get the database for a channel (thread-safe)
     * @return Shared pointer to database, or nullptr
     */
    std::shared_ptr<const DBCDatabase> database(int channelIndex) const;

    /**
     * @brief Get the DBC file path for a channel
     */
    QString dbcFilePath(int channelIndex) const;

    // === Encode / Decode Convenience ===

    /**
     * @brief Decode a CAN message using the DBC for the given channel
     * @param channelIndex CAN channel
     * @param canId CAN message ID
     * @param data Raw data bytes
     * @param dataLength Number of data bytes
     * @return Map of signal name → physical value, empty if not found
     */
    QMap<QString, double> decode(int channelIndex, uint32_t canId,
                                  const uint8_t* data, int dataLength) const;

    /**
     * @brief Encode signal values into raw CAN data
     * @param channelIndex CAN channel
     * @param canId CAN message ID
     * @param signalValues Map of signal name → physical value
     * @param data Output buffer
     * @param dataLength Buffer size
     * @return true if message was found and encoded
     */
    bool encode(int channelIndex, uint32_t canId,
                const QMap<QString, double>& signalValues,
                uint8_t* data, int dataLength) const;

    // === UI Helpers ===

    /**
     * @brief Get message display list for combo box ("0xID - MsgName")
     */
    QStringList messageDisplayList(int channelIndex) const;

    /**
     * @brief Get all message names for a channel
     */
    QStringList messageNames(int channelIndex) const;

    /**
     * @brief Get all signal names for a given message on a channel
     */
    QStringList signalNames(int channelIndex, uint32_t canId) const;

    /**
     * @brief Resolve a message display string to its CAN ID
     */
    uint32_t resolveMessageId(int channelIndex, const QString& displayString) const;

    // === Persistence ===

    /**
     * @brief Save DBC file paths to QSettings
     */
    void savePaths();

    /**
     * @brief Load DBC file paths from QSettings and trigger background parsing
     */
    void loadSavedPaths();

signals:
    /**
     * @brief Emitted when DBC loading starts
     */
    void loadStarted(int channelIndex, const QString& filePath);

    /**
     * @brief Emitted when DBC loading completes
     * @param channelIndex Channel index
     * @param success true if parsing succeeded
     * @param errorMsg Error message (empty on success)
     */
    void loadFinished(int channelIndex, bool success, const QString& errorMsg);

    /**
     * @brief Emitted when a DBC is unloaded
     */
    void databaseUnloaded(int channelIndex);

    /**
     * @brief Emitted on progress update during loading
     */
    void loadProgress(int channelIndex, const QString& status);

    /**
     * @brief Emitted when the message list changes (load/unload)
     * UI should refresh combo boxes when this fires.
     */
    void messageListChanged(int channelIndex);

private:
    DBCDatabaseManager();
    ~DBCDatabaseManager() override;
    DBCDatabaseManager(const DBCDatabaseManager&) = delete;
    DBCDatabaseManager& operator=(const DBCDatabaseManager&) = delete;

    void onWorkerFinished(int channelIndex, std::shared_ptr<DBCDatabase> database,
                          const QString& errorMsg);

    struct ChannelData {
        QString filePath;
        std::shared_ptr<DBCDatabase> database;
        bool loading = false;
    };

    std::array<ChannelData, MAX_CHANNELS> m_channels;
    mutable QMutex m_mutex;

    // Background worker
    QThread*        m_workerThread = nullptr;
    DBCLoadWorker*  m_worker       = nullptr;
};

} // namespace DBCManager
