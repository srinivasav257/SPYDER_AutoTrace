#pragma once
/**
 * @file CANManager.h
 * @brief Singleton CAN bus manager for multi-driver, multi-channel operation.
 *
 * The CANManager provides:
 *   - Registration and lifecycle management of CAN driver backends
 *   - Named channel slots (e.g. "CAN 1", "CAN 2") from HWConfigManager
 *   - Unified transmit/receive API across all driver types
 *   - Hardware detection aggregated across all registered drivers
 */

#include "CANInterface.h"
#include "VectorCANDriver.h"

#include <QObject>
#include <QMap>
#include <QMutex>
#include <memory>

namespace CANManager {

/**
 * @brief Singleton manager for CAN bus communication.
 *
 * Usage:
 * @code
 *   auto& can = CANBusManager::instance();
 *
 *   // Detect all Vector channels
 *   auto* vector = can.vectorDriver();
 *   if (vector && vector->isAvailable()) {
 *       auto channels = vector->detectChannels();
 *       // ... populate UI
 *   }
 *
 *   // Open a slot
 *   can.openSlot("CAN 1", driver, channelInfo, config);
 *   can.transmit("CAN 1", msg);
 *   can.closeSlot("CAN 1");
 * @endcode
 */
class CANBusManager : public QObject
{
    Q_OBJECT

public:
    static CANBusManager& instance();

    // === Driver access ===

    /** @brief Get the Vector XL driver instance (always created, may not be available). */
    VectorCANDriver* vectorDriver() { return m_vectorDriver.get(); }

    /** @brief Get a driver by name (e.g., "Vector XL"). */
    ICANDriver* driverByName(const QString& name) const;

    /** @brief List all registered driver names. */
    QStringList availableDriverNames() const;

    // === Aggregated hardware detection ===

    /**
     * @brief Detect channels across all available drivers.
     * @return Map of driver_name → list of channels
     */
    QMap<QString, QList<CANChannelInfo>> detectAllChannels();

    // === Channel Slot Management ===

    /**
     * @brief Open a named channel slot.
     * @param slotName   Logical name (e.g. "CAN 1")
     * @param driver     Driver to use
     * @param channel    Channel hardware info
     * @param config     Bus configuration
     */
    CANResult openSlot(const QString& slotName,
                       ICANDriver* driver,
                       const CANChannelInfo& channel,
                       const CANBusConfig& config);

    /** @brief Close a named channel slot. */
    void closeSlot(const QString& slotName);

    /** @brief Close all open slots. */
    void closeAllSlots();

    /** @brief Check if a slot is currently open. */
    bool isSlotOpen(const QString& slotName) const;

    /** @brief Get the driver associated with a slot. */
    ICANDriver* slotDriver(const QString& slotName) const;

    /** @brief List all currently open slot names. */
    QStringList openSlotNames() const;

    // === Data operations (by slot name) ===

    /** @brief Transmit a message on a named slot. */
    CANResult transmit(const QString& slotName, const CANMessage& msg);

    /** @brief Receive a message from a named slot. */
    CANResult receive(const QString& slotName, CANMessage& msg, int timeoutMs = 1000);

    /** @brief Flush receive queue on a named slot. */
    CANResult flushReceiveQueue(const QString& slotName);

signals:
    void slotOpened(const QString& slotName);
    void slotClosed(const QString& slotName);
    void errorOccurred(const QString& slotName, const QString& error);

private:
    CANBusManager();
    ~CANBusManager() override;
    CANBusManager(const CANBusManager&) = delete;
    CANBusManager& operator=(const CANBusManager&) = delete;

    // Registered drivers
    std::unique_ptr<VectorCANDriver> m_vectorDriver;
    // Future: std::unique_ptr<KvaserCANDriver> m_kvaserDriver;

    // Open channel slots: slotName → driver pointer
    struct SlotInfo {
        ICANDriver*  driver  = nullptr;
        CANChannelInfo channel;
    };
    QMap<QString, SlotInfo> m_slots;
    mutable QMutex m_mutex;
};

} // namespace CANManager
