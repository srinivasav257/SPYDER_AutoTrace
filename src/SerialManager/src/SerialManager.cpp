/**
 * @file SerialManager.cpp
 * @brief Implementation of SerialPortManager singleton.
 */

#include "SerialManager.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QThread>

namespace SerialManager {

//=============================================================================
// SerialPortConfig Helpers
//=============================================================================

QSerialPort::DataBits SerialPortConfig::dataBitsFromInt(int bits)
{
    switch (bits) {
        case 5: return QSerialPort::Data5;
        case 6: return QSerialPort::Data6;
        case 7: return QSerialPort::Data7;
        case 8: 
        default: return QSerialPort::Data8;
    }
}

QSerialPort::StopBits SerialPortConfig::stopBitsFromInt(int bits)
{
    switch (bits) {
        case 2: return QSerialPort::TwoStop;
        case 3: return QSerialPort::OneAndHalfStop;
        case 1:
        default: return QSerialPort::OneStop;
    }
}

QSerialPort::Parity SerialPortConfig::parityFromString(const QString& parity)
{
    QString p = parity.toLower();
    if (p == "even") return QSerialPort::EvenParity;
    if (p == "odd") return QSerialPort::OddParity;
    if (p == "space") return QSerialPort::SpaceParity;
    if (p == "mark") return QSerialPort::MarkParity;
    return QSerialPort::NoParity;
}

QSerialPort::FlowControl SerialPortConfig::flowControlFromString(const QString& flow)
{
    QString f = flow.toLower();
    if (f == "hardware" || f == "rts/cts") return QSerialPort::HardwareControl;
    if (f == "software" || f == "xon/xoff") return QSerialPort::SoftwareControl;
    return QSerialPort::NoFlowControl;
}

//=============================================================================
// SerialPortManager Singleton
//=============================================================================

SerialPortManager& SerialPortManager::instance()
{
    static SerialPortManager instance;
    return instance;
}

SerialPortManager::SerialPortManager()
    : QObject(nullptr)
{
    qDebug() << "SerialPortManager initialized";
}

SerialPortManager::~SerialPortManager()
{
    closeAllPorts();
    qDebug() << "SerialPortManager destroyed";
}

//=============================================================================
// Port Configuration
//=============================================================================

void SerialPortManager::setPortConfig(const QString& portName, const SerialPortConfig& config)
{
    QMutexLocker locker(&m_mutex);
    m_portConfigs[portName] = config;
    qDebug() << "Configuration set for port:" << portName 
             << "Baud:" << config.baudRate;
}

SerialPortConfig SerialPortManager::portConfig(const QString& portName) const
{
    QMutexLocker locker(&m_mutex);
    if (m_portConfigs.contains(portName)) {
        return m_portConfigs[portName];
    }
    // Return default config with the port name
    SerialPortConfig defaultConfig;
    defaultConfig.portName = portName;
    return defaultConfig;
}

bool SerialPortManager::hasPortConfig(const QString& portName) const
{
    QMutexLocker locker(&m_mutex);
    return m_portConfigs.contains(portName);
}

//=============================================================================
// Port Management
//=============================================================================

SerialResult SerialPortManager::openPort(const QString& portName)
{
    // Normalize port name (e.g., "com3" -> "COM3" on Windows)
    const QString normalizedName = normalizePortName(portName);
    
    QMutexLocker locker(&m_mutex);
    
    // Check if already open (with null-safety)
    if (m_openPorts.contains(normalizedName)) {
        auto& existing = m_openPorts[normalizedName];
        if (existing && existing->isOpen()) {
            qDebug() << "Port already open:" << normalizedName;
            return SerialResult::Success();
        }
        // Remove stale entry
        m_openPorts.erase(normalizedName);
    }
    
    // Check if port actually exists on the system
    if (!isPortAvailableOnSystem(normalizedName)) {
        QStringList available = availablePorts();
        QString error = QString("Port '%1' not found on system. Available ports: %2")
                            .arg(normalizedName)
                            .arg(available.isEmpty() ? "(none)" : available.join(", "));
        m_lastErrors[normalizedName] = error;
        qWarning() << error;
        emit errorOccurred(normalizedName, error);
        return SerialResult::Failure(error);
    }
    
    // Get configuration (also check under original name for backward compat)
    SerialPortConfig config;
    if (m_portConfigs.contains(normalizedName)) {
        config = m_portConfigs[normalizedName];
    } else if (m_portConfigs.contains(portName)) {
        config = m_portConfigs[portName];
        // Migrate to normalized name
        m_portConfigs[normalizedName] = config;
    } else {
        config.portName = normalizedName;
        m_portConfigs[normalizedName] = config;
    }
    
    // Create and configure port
    auto port = std::make_unique<QSerialPort>();
    port->setPortName(normalizedName);
    
    // Open the port first, then apply config (more reliable on some drivers)
    static constexpr int MAX_RETRIES = 3;
    static constexpr int RETRY_DELAY_MS = 100;
    bool opened = false;
    QString lastOpenError;
    
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt) {
        if (attempt > 0) {
            qDebug() << "Retry" << attempt << "opening port" << normalizedName;
            locker.unlock();
            QThread::msleep(RETRY_DELAY_MS);
            locker.relock();
        }
        
        if (port->open(QIODevice::ReadWrite)) {
            opened = true;
            break;
        }
        lastOpenError = port->errorString();
        port->close(); // Reset state before retry
    }
    
    if (!opened) {
        QString error = QString("Failed to open port %1 after %2 attempts: %3")
                            .arg(normalizedName)
                            .arg(MAX_RETRIES)
                            .arg(lastOpenError);
        m_lastErrors[normalizedName] = error;
        qWarning() << error;
        emit errorOccurred(normalizedName, error);
        return SerialResult::Failure(error);
    }
    
    // Apply configuration after opening (return values checked)
    QString configError;
    if (!applyConfig(port.get(), config, &configError)) {
        port->close();
        QString error = QString("Port %1 opened but configuration failed: %2")
                            .arg(normalizedName)
                            .arg(configError);
        m_lastErrors[normalizedName] = error;
        qWarning() << error;
        emit errorOccurred(normalizedName, error);
        return SerialResult::Failure(error);
    }
    
    // Set DTR and RTS signals (required by some devices)
    port->setDataTerminalReady(true);
    port->setRequestToSend(true);
    
    qDebug() << "Port opened:" << normalizedName << "Baud:" << config.baudRate;
    m_openPorts[normalizedName] = std::move(port);
    
    locker.unlock();
    emit portOpened(normalizedName);
    
    return SerialResult::Success();
}

SerialResult SerialPortManager::openPort(const SerialPortConfig& config)
{
    const QString normalizedName = normalizePortName(config.portName);
    SerialPortConfig normalizedConfig = config;
    normalizedConfig.portName = normalizedName;
    setPortConfig(normalizedName, normalizedConfig);
    return openPort(normalizedName);
}

void SerialPortManager::closePort(const QString& portName)
{
    const QString normalizedName = normalizePortName(portName);
    QMutexLocker locker(&m_mutex);
    
    if (m_openPorts.contains(normalizedName)) {
        auto& port = m_openPorts[normalizedName];
        if (port && port->isOpen()) {
            port->setDataTerminalReady(false);
            port->setRequestToSend(false);
            port->close();
            qDebug() << "Port closed:" << normalizedName;
        }
        m_openPorts.erase(normalizedName);
        
        locker.unlock();
        emit portClosed(normalizedName);
    }
}

void SerialPortManager::closeAllPorts()
{
    QMutexLocker locker(&m_mutex);
    
    QStringList ports;
    for (const auto& [name, port] : m_openPorts) {
        ports.append(name);
        if (port && port->isOpen()) {
            port->close();
            qDebug() << "Port closed:" << name;
        }
    }
    m_openPorts.clear();
    
    locker.unlock();
    
    for (const QString& portName : ports) {
        emit portClosed(portName);
    }
}

bool SerialPortManager::isPortOpen(const QString& portName) const
{
    const QString normalizedName = normalizePortName(portName);
    QMutexLocker locker(&m_mutex);
    auto it = m_openPorts.find(normalizedName);
    return it != m_openPorts.end() && it->second && it->second->isOpen();
}

QStringList SerialPortManager::openPorts() const
{
    QMutexLocker locker(&m_mutex);
    QStringList result;
    for (auto it = m_openPorts.begin(); it != m_openPorts.end(); ++it) {
        if (it->second && it->second->isOpen()) {
            result.append(it->first);
        }
    }
    return result;
}

QStringList SerialPortManager::availablePorts()
{
    QStringList result;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : ports) {
        result.append(info.portName());
    }
    return result;
}

//=============================================================================
// Data Operations
//=============================================================================

SerialResult SerialPortManager::send(const QString& portName, const QByteArray& data)
{
    QSerialPort* port = getPort(portName, true);
    if (!port) {
        QString error = "Failed to get port: " + portName;
        return SerialResult::Failure(error);
    }
    
    if (!port->isOpen()) {
        QString error = "Port is not open: " + portName;
        return SerialResult::Failure(error);
    }
    
    qint64 bytesWritten = port->write(data);
    if (bytesWritten == -1) {
        QString error = QString("Write failed on %1: %2")
                            .arg(portName)
                            .arg(port->errorString());
        m_lastErrors[portName] = error;
        emit errorOccurred(portName, error);
        return SerialResult::Failure(error);
    }
    
    // Wait for data to be written
    SerialPortConfig config = portConfig(portName);
    if (!port->waitForBytesWritten(config.writeTimeoutMs)) {
        QString error = QString("Write timeout on %1").arg(portName);
        m_lastErrors[portName] = error;
        emit errorOccurred(portName, error);
        return SerialResult::Failure(error);
    }
    
    qDebug() << "Sent" << bytesWritten << "bytes to" << portName;
    emit dataSent(portName, data);
    
    return SerialResult::Success({}, static_cast<int>(bytesWritten));
}

SerialResult SerialPortManager::send(const QString& portName, const QString& data)
{
    return send(portName, data.toUtf8());
}

SerialResult SerialPortManager::read(const QString& portName, int timeoutMs)
{
    QSerialPort* port = getPort(portName, false);
    if (!port || !port->isOpen()) {
        return SerialResult::Failure("Port not open: " + portName);
    }
    
    SerialPortConfig config = portConfig(portName);
    int timeout = (timeoutMs > 0) ? timeoutMs : config.readTimeoutMs;
    
    QByteArray receivedData;
    QElapsedTimer timer;
    timer.start();
    
    while (timer.elapsed() < timeout) {
        if (port->waitForReadyRead(100)) {
            receivedData.append(port->readAll());
        }
        QCoreApplication::processEvents();
    }
    
    // Read any remaining data
    receivedData.append(port->readAll());
    
    if (!receivedData.isEmpty()) {
        emit dataReceived(portName, receivedData);
    }
    
    return SerialResult::Success(receivedData);
}

SerialResult SerialPortManager::readUntil(const QString& portName, const QByteArray& pattern, int timeoutMs)
{
    QSerialPort* port = getPort(portName, false);
    if (!port || !port->isOpen()) {
        return SerialResult::Failure("Port not open: " + portName);
    }
    
    SerialPortConfig config = portConfig(portName);
    int timeout = (timeoutMs > 0) ? timeoutMs : config.readTimeoutMs;
    
    QByteArray receivedData;
    QElapsedTimer timer;
    timer.start();
    
    while (timer.elapsed() < timeout) {
        if (port->waitForReadyRead(50)) {
            receivedData.append(port->readAll());
            
            // Check if pattern is found
            if (receivedData.contains(pattern)) {
                emit dataReceived(portName, receivedData);
                return SerialResult::Success(receivedData);
            }
        }
        QCoreApplication::processEvents();
    }
    
    // Final read
    receivedData.append(port->readAll());
    
    if (!receivedData.isEmpty()) {
        emit dataReceived(portName, receivedData);
    }
    
    if (receivedData.contains(pattern)) {
        return SerialResult::Success(receivedData);
    }
    
    return SerialResult::MatchFailure("Pattern not found within timeout", receivedData);
}

SerialResult SerialPortManager::sendAndMatchResponse(const QString& portName,
                                                      const QByteArray& sendData,
                                                      const QString& expectedResponse,
                                                      int timeoutMs)
{
    // Ensure port is open
    QSerialPort* port = getPort(portName, true);
    if (!port) {
        return SerialResult::Failure("Failed to open port: " + portName);
    }
    
    if (!port->isOpen()) {
        return SerialResult::Failure("Port is not open: " + portName);
    }
    
    // Clear any pending data
    port->clear();
    
    // Send data
    qint64 bytesWritten = port->write(sendData);
    if (bytesWritten == -1) {
        QString error = QString("Write failed: %1").arg(port->errorString());
        m_lastErrors[portName] = error;
        emit errorOccurred(portName, error);
        return SerialResult::Failure(error);
    }
    
    SerialPortConfig config = portConfig(portName);
    if (!port->waitForBytesWritten(config.writeTimeoutMs)) {
        return SerialResult::Failure("Write timeout");
    }
    
    qDebug() << "Sent" << bytesWritten << "bytes to" << portName 
             << "waiting for:" << expectedResponse;
    emit dataSent(portName, sendData);
    
    // Read response and look for match
    QByteArray receivedData;
    QElapsedTimer timer;
    timer.start();
    
    while (timer.elapsed() < timeoutMs) {
        if (port->waitForReadyRead(50)) {
            receivedData.append(port->readAll());
            
            // Check if expected response is found
            QString receivedStr = QString::fromUtf8(receivedData);
            if (receivedStr.contains(expectedResponse, Qt::CaseInsensitive)) {
                qDebug() << "Match found:" << expectedResponse << "in response";
                emit dataReceived(portName, receivedData);
                return SerialResult::MatchSuccess(receivedData);
            }
        }
        QCoreApplication::processEvents();
    }
    
    // Final read after timeout
    receivedData.append(port->readAll());
    
    if (!receivedData.isEmpty()) {
        emit dataReceived(portName, receivedData);
    }
    
    // Check one more time
    QString receivedStr = QString::fromUtf8(receivedData);
    if (receivedStr.contains(expectedResponse, Qt::CaseInsensitive)) {
        return SerialResult::MatchSuccess(receivedData);
    }
    
    QString error = QString("Expected response '%1' not found. Received: %2")
                        .arg(expectedResponse)
                        .arg(QString::fromUtf8(receivedData.left(200)));
    m_lastErrors[portName] = error;
    
    return SerialResult::MatchFailure(error, receivedData);
}

SerialResult SerialPortManager::sendAndMatchResponse(const QString& portName,
                                                      const QString& sendData,
                                                      const QString& expectedResponse,
                                                      int timeoutMs)
{
    return sendAndMatchResponse(portName, sendData.toUtf8(), expectedResponse, timeoutMs);
}

//=============================================================================
// Utility
//=============================================================================

bool SerialPortManager::clearBuffers(const QString& portName)
{
    const QString normalizedName = normalizePortName(portName);
    QMutexLocker locker(&m_mutex);
    
    if (m_openPorts.contains(normalizedName) && m_openPorts[normalizedName]) {
        return m_openPorts[normalizedName]->clear();
    }
    return false;
}

QString SerialPortManager::lastError(const QString& portName) const
{
    const QString normalizedName = normalizePortName(portName);
    QMutexLocker locker(&m_mutex);
    return m_lastErrors.value(normalizedName);
}

QString SerialPortManager::normalizePortName(const QString& portName)
{
#ifdef Q_OS_WIN
    // Windows: normalize COM port names to uppercase ("com3" -> "COM3")
    QString name = portName.trimmed();
    if (name.toLower().startsWith("com")) {
        return name.toUpper();
    }
    return name;
#else
    return portName.trimmed();
#endif
}

bool SerialPortManager::isPortAvailableOnSystem(const QString& portName)
{
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : ports) {
        if (info.portName().compare(portName, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

//=============================================================================
// Private Methods
//=============================================================================

QSerialPort* SerialPortManager::getPort(const QString& portName, bool autoOpen)
{
    const QString normalizedName = normalizePortName(portName);
    QMutexLocker locker(&m_mutex);
    
    // Check if port exists and is open (null-safe)
    auto it = m_openPorts.find(normalizedName);
    if (it != m_openPorts.end() && it->second && it->second->isOpen()) {
        return it->second.get();
    }
    
    // Auto-open if requested
    if (autoOpen) {
        locker.unlock();
        SerialResult result = openPort(normalizedName);
        locker.relock();
        
        auto it2 = m_openPorts.find(normalizedName);
        if (result.success && it2 != m_openPorts.end() && it2->second) {
            return it2->second.get();
        }
    }
    
    return nullptr;
}

bool SerialPortManager::applyConfig(QSerialPort* port, const SerialPortConfig& config, QString* errorOut)
{
    if (!port) {
        if (errorOut) *errorOut = "Null port pointer";
        return false;
    }
    
    QStringList errors;
    
    if (!port->setBaudRate(config.baudRate))
        errors << QString("setBaudRate(%1) failed: %2").arg(config.baudRate).arg(port->errorString());
    
    if (!port->setDataBits(config.dataBits))
        errors << QString("setDataBits failed: %1").arg(port->errorString());
    
    if (!port->setStopBits(config.stopBits))
        errors << QString("setStopBits failed: %1").arg(port->errorString());
    
    if (!port->setParity(config.parity))
        errors << QString("setParity failed: %1").arg(port->errorString());
    
    if (!port->setFlowControl(config.flowControl))
        errors << QString("setFlowControl failed: %1").arg(port->errorString());
    
    if (!errors.isEmpty()) {
        QString combined = errors.join("; ");
        qWarning() << "Config errors on" << config.portName << ":" << combined;
        if (errorOut) *errorOut = combined;
        return false;
    }
    
    return true;
}

} // namespace SerialManager
