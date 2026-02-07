#include "HWConfigManager.h"
#include <SerialManager.h>
#include <QMutexLocker>

HWConfigManager& HWConfigManager::instance()
{
    static HWConfigManager inst;
    return inst;
}

HWConfigManager::HWConfigManager()
{
    // Set default custom names
    m_serialDebugPorts[0].customName = "Debug Port 1";
    m_serialDebugPorts[1].customName = "Debug Port 2";
    m_serialDebugPorts[2].customName = "Debug Port 3";
    m_serialDebugPorts[3].customName = "Debug Port 4";

    // Set default CAN custom names
    m_canPorts[0].customName = "CAN 1";
    m_canPorts[1].customName = "CAN 2";

    // Set default power supply baud rate (TENMA uses 9600)
    m_powerSupply.serial.baudRate = 9600;

    // Set default Modbus baud rate
    m_modbusRelay.serial.baudRate = 9600;

    load();
}

// ---------------------------------------------------------------------------
// Serial Debug Ports
// ---------------------------------------------------------------------------

SerialDebugPortConfig HWConfigManager::serialDebugPort(int index) const
{
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index >= SERIAL_PORT_COUNT)
        return {};
    return m_serialDebugPorts[index];
}

void HWConfigManager::setSerialDebugPort(int index, const SerialDebugPortConfig& config)
{
    {
        QMutexLocker locker(&m_mutex);
        if (index < 0 || index >= SERIAL_PORT_COUNT)
            return;
        m_serialDebugPorts[index] = config;
    }
    emit configChanged();
}

// ---------------------------------------------------------------------------
// CAN Ports
// ---------------------------------------------------------------------------

CANPortConfig HWConfigManager::canPort(int index) const
{
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index >= CAN_PORT_COUNT)
        return {};
    return m_canPorts[index];
}

void HWConfigManager::setCanPort(int index, const CANPortConfig& config)
{
    {
        QMutexLocker locker(&m_mutex);
        if (index < 0 || index >= CAN_PORT_COUNT)
            return;
        m_canPorts[index] = config;
    }
    emit configChanged();
}

// ---------------------------------------------------------------------------
// Power Supply / Modbus
// ---------------------------------------------------------------------------

PowerSupplyConfig HWConfigManager::powerSupply() const { QMutexLocker locker(&m_mutex); return m_powerSupply; }
void HWConfigManager::setPowerSupply(const PowerSupplyConfig& config) {
    { QMutexLocker locker(&m_mutex); m_powerSupply = config; }
    emit configChanged();
}

ModbusRelayConfig HWConfigManager::modbusRelay() const { QMutexLocker locker(&m_mutex); return m_modbusRelay; }
void HWConfigManager::setModbusRelay(const ModbusRelayConfig& config) {
    { QMutexLocker locker(&m_mutex); m_modbusRelay = config; }
    emit configChanged();
}

// ---------------------------------------------------------------------------
// Alias Resolution
// ---------------------------------------------------------------------------

QStringList HWConfigManager::allPortAliases() const
{
    QMutexLocker locker(&m_mutex);
    QStringList aliases;
    for (int i = 0; i < SERIAL_PORT_COUNT; ++i) {
        const auto& p = m_serialDebugPorts[i];
        if (!p.customName.isEmpty() && !p.serial.portName.isEmpty()) {
            aliases.append(QString("%1 (%2)").arg(p.customName, p.serial.portName));
        }
    }
    // Power Supply
    if (!m_powerSupply.customName.isEmpty() && !m_powerSupply.serial.portName.isEmpty())
        aliases.append(QString("%1 (%2)").arg(m_powerSupply.customName, m_powerSupply.serial.portName));
    // Modbus Relay
    if (!m_modbusRelay.customName.isEmpty() && !m_modbusRelay.serial.portName.isEmpty())
        aliases.append(QString("%1 (%2)").arg(m_modbusRelay.customName, m_modbusRelay.serial.portName));
    return aliases;
}

QString HWConfigManager::resolvePortAlias(const QString& alias) const
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < SERIAL_PORT_COUNT; ++i) {
        const auto& p = m_serialDebugPorts[i];
        if (p.customName == alias)
            return p.serial.portName;
        if (alias == QString("%1 (%2)").arg(p.customName, p.serial.portName))
            return p.serial.portName;
    }
    // Power Supply
    if (m_powerSupply.customName == alias
        || alias == QString("%1 (%2)").arg(m_powerSupply.customName, m_powerSupply.serial.portName))
        return m_powerSupply.serial.portName;
    // Modbus Relay
    if (m_modbusRelay.customName == alias
        || alias == QString("%1 (%2)").arg(m_modbusRelay.customName, m_modbusRelay.serial.portName))
        return m_modbusRelay.serial.portName;
    return alias; // return as-is if not a known alias
}

// ---------------------------------------------------------------------------
// CAN Alias Resolution
// ---------------------------------------------------------------------------

QStringList HWConfigManager::allCANAliases() const
{
    QMutexLocker locker(&m_mutex);
    QStringList aliases;
    for (int i = 0; i < CAN_PORT_COUNT; ++i) {
        const auto& c = m_canPorts[i];
        if (!c.customName.isEmpty()) {
            aliases.append(c.customName);
        }
    }
    return aliases;
}

QString HWConfigManager::resolveCANAlias(const QString& alias) const
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < CAN_PORT_COUNT; ++i) {
        if (m_canPorts[i].customName == alias) {
            return m_canPorts[i].device;
        }
    }
    return alias;
}

// ---------------------------------------------------------------------------
// Persistence helpers
// ---------------------------------------------------------------------------

static QString parityToString(QSerialPort::Parity p)
{
    switch (p) {
    case QSerialPort::EvenParity:  return "Even";
    case QSerialPort::OddParity:   return "Odd";
    case QSerialPort::SpaceParity: return "Space";
    case QSerialPort::MarkParity:  return "Mark";
    default:                       return "None";
    }
}

static QString stopBitsToString(QSerialPort::StopBits s)
{
    switch (s) {
    case QSerialPort::OneAndHalfStop: return "1.5";
    case QSerialPort::TwoStop:       return "2";
    default:                         return "1";
    }
}

static QString flowControlToString(QSerialPort::FlowControl f)
{
    switch (f) {
    case QSerialPort::HardwareControl: return "Hardware";
    case QSerialPort::SoftwareControl: return "Software";
    default:                           return "None";
    }
}

void HWConfigManager::saveSerialConfig(QSettings& s, const QString& prefix,
                                        const SerialManager::SerialPortConfig& cfg)
{
    s.setValue(prefix + "/portName", cfg.portName);
    s.setValue(prefix + "/baudRate", cfg.baudRate);
    s.setValue(prefix + "/dataBits", static_cast<int>(cfg.dataBits));
    s.setValue(prefix + "/parity", parityToString(cfg.parity));
    s.setValue(prefix + "/stopBits", stopBitsToString(cfg.stopBits));
    s.setValue(prefix + "/flowControl", flowControlToString(cfg.flowControl));
}

SerialManager::SerialPortConfig HWConfigManager::loadSerialConfig(QSettings& s, const QString& prefix)
{
    SerialManager::SerialPortConfig cfg;
    cfg.portName = s.value(prefix + "/portName", cfg.portName).toString();
    cfg.baudRate = s.value(prefix + "/baudRate", cfg.baudRate).toInt();
    cfg.dataBits = SerialManager::SerialPortConfig::dataBitsFromInt(
        s.value(prefix + "/dataBits", static_cast<int>(cfg.dataBits)).toInt());
    cfg.parity = SerialManager::SerialPortConfig::parityFromString(
        s.value(prefix + "/parity", "None").toString());
    QString stopBitsStr = s.value(prefix + "/stopBits", "1").toString();
    if (stopBitsStr == "1.5")
        cfg.stopBits = QSerialPort::OneAndHalfStop;
    else if (stopBitsStr == "2")
        cfg.stopBits = QSerialPort::TwoStop;
    else
        cfg.stopBits = QSerialPort::OneStop;
    cfg.flowControl = SerialManager::SerialPortConfig::flowControlFromString(
        s.value(prefix + "/flowControl", "None").toString());
    return cfg;
}

// ---------------------------------------------------------------------------
// Save / Load
// ---------------------------------------------------------------------------

void HWConfigManager::save()
{
    QMutexLocker locker(&m_mutex);
    QSettings s;
    s.beginGroup("HWConfig");

    // Write schema version first
    s.setValue("schemaVersion", CONFIG_SCHEMA_VERSION);

    // Serial Debug Ports
    for (int i = 0; i < SERIAL_PORT_COUNT; ++i) {
        QString key = QString("SerialDebug/%1").arg(i);
        s.setValue(key + "/customName", m_serialDebugPorts[i].customName);
        saveSerialConfig(s, key, m_serialDebugPorts[i].serial);
    }

    // CAN Ports
    for (int i = 0; i < CAN_PORT_COUNT; ++i) {
        QString key = QString("CAN/%1").arg(i);
        s.setValue(key + "/customName", m_canPorts[i].customName);
        s.setValue(key + "/interfaceType", m_canPorts[i].interfaceType);
        s.setValue(key + "/device", m_canPorts[i].device);
        s.setValue(key + "/channel", m_canPorts[i].channel);
        s.setValue(key + "/bitrate", m_canPorts[i].bitrate);
        s.setValue(key + "/fdEnabled", m_canPorts[i].fdEnabled);
        s.setValue(key + "/fdDataBitrate", m_canPorts[i].fdDataBitrate);
        // Vector XL channel mapping
        s.setValue(key + "/vectorHwType", m_canPorts[i].vectorHwType);
        s.setValue(key + "/vectorHwIndex", m_canPorts[i].vectorHwIndex);
        s.setValue(key + "/vectorHwChannel", m_canPorts[i].vectorHwChannel);
        s.setValue(key + "/vectorChannelIdx", m_canPorts[i].vectorChannelIdx);
        s.setValue(key + "/vectorChannelMask", QVariant::fromValue(m_canPorts[i].vectorChannelMask));
        s.setValue(key + "/dbcFilePath", m_canPorts[i].dbcFilePath);
    }

    // Power Supply
    s.setValue("PowerSupply/customName", m_powerSupply.customName);
    saveSerialConfig(s, "PowerSupply", m_powerSupply.serial);

    // Modbus Relay
    s.setValue("ModbusRelay/customName", m_modbusRelay.customName);
    saveSerialConfig(s, "ModbusRelay", m_modbusRelay.serial);
    s.setValue("ModbusRelay/modbusAddress", m_modbusRelay.modbusAddress);

    s.endGroup();
}

void HWConfigManager::load()
{
    QMutexLocker locker(&m_mutex);
    QSettings s;
    s.beginGroup("HWConfig");

    // Read and validate schema version.
    // If the stored version is newer than what we understand, skip loading
    // to avoid corrupting state with misinterpreted keys.
    const int storedVersion = s.value("schemaVersion", 0).toInt();
    if (storedVersion > CONFIG_SCHEMA_VERSION) {
        qWarning() << "HWConfigManager: stored schema version" << storedVersion
                   << "is newer than supported" << CONFIG_SCHEMA_VERSION
                   << "— skipping load";
        s.endGroup();
        return;
    }

    // Serial Debug Ports
    for (int i = 0; i < SERIAL_PORT_COUNT; ++i) {
        QString key = QString("SerialDebug/%1").arg(i);
        if (s.contains(key + "/customName")) {
            m_serialDebugPorts[i].customName = s.value(key + "/customName").toString();
            m_serialDebugPorts[i].serial = loadSerialConfig(s, key);
        }
    }

    // CAN Ports
    for (int i = 0; i < CAN_PORT_COUNT; ++i) {
        QString key = QString("CAN/%1").arg(i);
        if (s.contains(key + "/interfaceType")) {
            m_canPorts[i].customName = s.value(key + "/customName").toString();
            m_canPorts[i].interfaceType = s.value(key + "/interfaceType").toString();
            m_canPorts[i].device = s.value(key + "/device").toString();
            m_canPorts[i].channel = s.value(key + "/channel", 1).toInt();
            m_canPorts[i].bitrate = s.value(key + "/bitrate").toInt();
            m_canPorts[i].fdEnabled = s.value(key + "/fdEnabled").toBool();
            m_canPorts[i].fdDataBitrate = s.value(key + "/fdDataBitrate").toInt();
            // Vector XL channel mapping
            m_canPorts[i].vectorHwType = s.value(key + "/vectorHwType", 0).toInt();
            m_canPorts[i].vectorHwIndex = s.value(key + "/vectorHwIndex", 0).toInt();
            m_canPorts[i].vectorHwChannel = s.value(key + "/vectorHwChannel", 0).toInt();
            m_canPorts[i].vectorChannelIdx = s.value(key + "/vectorChannelIdx", -1).toInt();
            m_canPorts[i].vectorChannelMask = s.value(key + "/vectorChannelMask", 0).toULongLong();
            m_canPorts[i].dbcFilePath = s.value(key + "/dbcFilePath").toString();
        }
    }

    // Power Supply
    if (s.contains("PowerSupply/portName")) {
        m_powerSupply.customName = s.value("PowerSupply/customName", m_powerSupply.customName).toString();
        m_powerSupply.serial = loadSerialConfig(s, "PowerSupply");
    }

    // Modbus Relay
    if (s.contains("ModbusRelay/portName")) {
        m_modbusRelay.customName = s.value("ModbusRelay/customName", m_modbusRelay.customName).toString();
        m_modbusRelay.serial = loadSerialConfig(s, "ModbusRelay");
        m_modbusRelay.modbusAddress = s.value("ModbusRelay/modbusAddress", 1).toInt();
    }

    s.endGroup();
}

void HWConfigManager::applyToSerialManager()
{
    QMutexLocker locker(&m_mutex);
    auto& serial = SerialManager::SerialPortManager::instance();
    for (int i = 0; i < SERIAL_PORT_COUNT; ++i) {
        const auto& p = m_serialDebugPorts[i];
        if (!p.serial.portName.isEmpty())
            serial.setPortConfig(p.serial.portName, p.serial);
    }
    if (!m_powerSupply.serial.portName.isEmpty())
        serial.setPortConfig(m_powerSupply.serial.portName, m_powerSupply.serial);
    if (!m_modbusRelay.serial.portName.isEmpty())
        serial.setPortConfig(m_modbusRelay.serial.portName, m_modbusRelay.serial);
}
