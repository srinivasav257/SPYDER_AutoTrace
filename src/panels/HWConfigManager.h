#pragma once

#include <SerialManager.h>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <array>

/**
 * @brief Configuration for a serial debug port (extends SerialPortConfig with custom name).
 */
struct SerialDebugPortConfig
{
    QString customName;                         ///< User-defined alias (e.g., "DUT Debug")
    SerialManager::SerialPortConfig serial;     ///< Underlying serial config
};

/**
 * @brief Configuration for a CAN port.
 */
struct CANPortConfig
{
    QString interfaceType = "PEAK PCAN";        ///< PEAK PCAN, Vector, SocketCAN, Custom
    QString device = "PCAN_USBBUS1";            ///< Device identifier
    int bitrate = 500000;                       ///< Nominal bitrate (bps)
    bool fdEnabled = false;                     ///< CAN FD mode
    int fdDataBitrate = 2000000;                ///< FD data phase bitrate (bps)
};

/**
 * @brief Configuration for the power supply (serial-connected).
 */
struct PowerSupplyConfig
{
    QString customName = "Power Supply";        ///< User-defined alias
    SerialManager::SerialPortConfig serial;     ///< Serial config for TENMA 72-2540
};

/**
 * @brief Configuration for the Modbus serial relay.
 */
struct ModbusRelayConfig
{
    QString customName = "Modbus Relay";        ///< User-defined alias
    SerialManager::SerialPortConfig serial;     ///< Serial config
    int modbusAddress = 1;                      ///< Modbus slave address (1-247)
};

/**
 * @brief Singleton that manages all hardware configuration with QSettings persistence.
 *
 * Provides access to serial debug port configs (with custom aliases),
 * CAN port configs, power supply config, and Modbus relay config.
 */
class HWConfigManager : public QObject
{
    Q_OBJECT

public:
    static constexpr int SERIAL_PORT_COUNT = 4;
    static constexpr int CAN_PORT_COUNT = 2;

    static HWConfigManager& instance();

    // --- Serial Debug Ports ---
    SerialDebugPortConfig serialDebugPort(int index) const;
    void setSerialDebugPort(int index, const SerialDebugPortConfig& config);

    // --- CAN Ports ---
    CANPortConfig canPort(int index) const;
    void setCanPort(int index, const CANPortConfig& config);

    // --- Power Supply ---
    PowerSupplyConfig powerSupply() const;
    void setPowerSupply(const PowerSupplyConfig& config);

    // --- Modbus Relay ---
    ModbusRelayConfig modbusRelay() const;
    void setModbusRelay(const ModbusRelayConfig& config);

    // --- Alias Resolution ---
    /** @brief Returns all non-empty custom names with format "Name (COMx)" */
    QStringList allPortAliases() const;

    /** @brief Resolves a custom alias to actual COM port name. Returns alias if not found. */
    QString resolvePortAlias(const QString& alias) const;

    // --- Persistence ---
    void save();
    void load();

    /** @brief Push serial configs to SerialPortManager */
    void applyToSerialManager();

signals:
    void configChanged();

private:
    HWConfigManager();

    void saveSerialConfig(QSettings& s, const QString& prefix,
                          const SerialManager::SerialPortConfig& cfg);
    SerialManager::SerialPortConfig loadSerialConfig(QSettings& s, const QString& prefix);

    std::array<SerialDebugPortConfig, SERIAL_PORT_COUNT> m_serialDebugPorts;
    std::array<CANPortConfig, CAN_PORT_COUNT> m_canPorts;
    PowerSupplyConfig m_powerSupply;
    ModbusRelayConfig m_modbusRelay;
};
