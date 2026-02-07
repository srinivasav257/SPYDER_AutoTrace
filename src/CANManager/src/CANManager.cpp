/**
 * @file CANManager.cpp
 * @brief Singleton CAN bus manager implementation.
 */

#include "CANManager.h"

#include <QDebug>

namespace CANManager {

// ============================================================================
//  Singleton
// ============================================================================

CANBusManager& CANBusManager::instance()
{
    static CANBusManager inst;
    return inst;
}

CANBusManager::CANBusManager()
    : QObject(nullptr)
{
    // Create driver instances (lazy initialization of actual DLL loading)
    m_vectorDriver = std::make_unique<VectorCANDriver>();

    qDebug() << "[CANManager] Initialized";
}

CANBusManager::~CANBusManager()
{
    closeAllSlots();

    // Shutdown drivers
    if (m_vectorDriver)
        m_vectorDriver->shutdown();

    qDebug() << "[CANManager] Destroyed";
}

// ============================================================================
//  Driver Access
// ============================================================================

ICANDriver* CANBusManager::driverByName(const QString& name) const
{
    if (m_vectorDriver && name == m_vectorDriver->driverName())
        return m_vectorDriver.get();
    // Future: check Kvaser, SocketCAN, etc.
    return nullptr;
}

QStringList CANBusManager::availableDriverNames() const
{
    QStringList names;
    if (m_vectorDriver)
        names.append(m_vectorDriver->driverName());
    // Future: add other drivers
    return names;
}

// ============================================================================
//  Aggregated Detection
// ============================================================================

QMap<QString, QList<CANChannelInfo>> CANBusManager::detectAllChannels()
{
    QMap<QString, QList<CANChannelInfo>> result;

    // Vector
    if (m_vectorDriver) {
        if (!m_vectorDriver->isAvailable()) {
            qDebug() << "[CANManager] Vector XL Library not available";
        } else {
            if (!m_vectorDriver->initialize()) {
                qWarning() << "[CANManager] Vector driver init failed:"
                           << m_vectorDriver->lastError();
            } else {
                auto channels = m_vectorDriver->detectChannels();
                if (!channels.isEmpty())
                    result[m_vectorDriver->driverName()] = channels;
            }
        }
    }

    // Future: Kvaser, SocketCAN, etc.

    return result;
}

// ============================================================================
//  Slot Management
// ============================================================================

CANResult CANBusManager::openSlot(const QString& slotName,
                                   ICANDriver* driver,
                                   const CANChannelInfo& channel,
                                   const CANBusConfig& config)
{
    QMutexLocker locker(&m_mutex);

    if (!driver)
        return CANResult::Failure("No driver specified");

    if (m_slots.contains(slotName)) {
        // Close existing slot first
        locker.unlock();
        closeSlot(slotName);
        locker.relock();
    }

    // Initialize driver if needed
    if (!driver->isOpen()) {
        // Driver may need initialization before opening a channel
    }

    CANResult result = driver->openChannel(channel, config);
    if (!result.success)
        return result;

    SlotInfo info;
    info.driver  = driver;
    info.channel = channel;
    m_slots[slotName] = info;

    qDebug() << "[CANManager] Slot opened:" << slotName
             << "via" << driver->driverName()
             << "on" << channel.name;

    locker.unlock();
    emit slotOpened(slotName);

    return CANResult::Success();
}

void CANBusManager::closeSlot(const QString& slotName)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_slots.find(slotName);
    if (it == m_slots.end())
        return;

    auto& info = it.value();
    if (info.driver)
        info.driver->closeChannel();

    m_slots.erase(it);

    qDebug() << "[CANManager] Slot closed:" << slotName;

    locker.unlock();
    emit slotClosed(slotName);
}

void CANBusManager::closeAllSlots()
{
    QMutexLocker locker(&m_mutex);

    QStringList names = m_slots.keys();
    locker.unlock();

    for (const auto& name : names)
        closeSlot(name);
}

bool CANBusManager::isSlotOpen(const QString& slotName) const
{
    QMutexLocker locker(&m_mutex);
    return m_slots.contains(slotName);
}

ICANDriver* CANBusManager::slotDriver(const QString& slotName) const
{
    QMutexLocker locker(&m_mutex);
    auto it = m_slots.find(slotName);
    return (it != m_slots.end()) ? it->driver : nullptr;
}

QStringList CANBusManager::openSlotNames() const
{
    QMutexLocker locker(&m_mutex);
    return m_slots.keys();
}

// ============================================================================
//  Data Operations
// ============================================================================

CANResult CANBusManager::transmit(const QString& slotName, const CANMessage& msg)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_slots.find(slotName);
    if (it == m_slots.end())
        return CANResult::Failure(QString("Slot '%1' not open").arg(slotName));

    if (!it->driver)
        return CANResult::Failure("Slot has no driver");

    return it->driver->transmit(msg);
}

CANResult CANBusManager::receive(const QString& slotName, CANMessage& msg, int timeoutMs)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_slots.find(slotName);
    if (it == m_slots.end())
        return CANResult::Failure(QString("Slot '%1' not open").arg(slotName));

    if (!it->driver)
        return CANResult::Failure("Slot has no driver");

    return it->driver->receive(msg, timeoutMs);
}

CANResult CANBusManager::flushReceiveQueue(const QString& slotName)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_slots.find(slotName);
    if (it == m_slots.end())
        return CANResult::Failure(QString("Slot '%1' not open").arg(slotName));

    if (!it->driver)
        return CANResult::Failure("Slot has no driver");

    return it->driver->flushReceiveQueue();
}

} // namespace CANManager
