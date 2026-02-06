/**
 * @file SerialManager.cpp
 * @brief Implementation of SerialPortManager singleton.
 */

#include "SerialManager.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>

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
    QMutexLocker locker(&m_mutex);
    
    // Check if already open
    if (m_openPorts.contains(portName) && m_openPorts[portName]->isOpen()) {
        qDebug() << "Port already open:" << portName;
        return SerialResult::Success();
    }
    
    // Get configuration
    SerialPortConfig config;
    if (m_portConfigs.contains(portName)) {
        config = m_portConfigs[portName];
    } else {
        config.portName = portName;
        m_portConfigs[portName] = config;
    }
    
    // Create and configure port
    auto port = std::make_unique<QSerialPort>();
    port->setPortName(portName);
    
    if (!applyConfig(port.get(), config)) {
        QString error = "Failed to configure port: " + portName;
        m_lastErrors[portName] = error;
        return SerialResult::Failure(error);
    }
    
    // Open the port
    if (!port->open(QIODevice::ReadWrite)) {
        QString error = QString("Failed to open port %1: %2")
                            .arg(portName)
                            .arg(port->errorString());
        m_lastErrors[portName] = error;
        qWarning() << error;
        emit errorOccurred(portName, error);
        return SerialResult::Failure(error);
    }
    
    qDebug() << "Port opened:" << portName << "Baud:" << config.baudRate;
    m_openPorts[portName] = std::move(port);
    
    locker.unlock();
    emit portOpened(portName);
    
    return SerialResult::Success();
}

SerialResult SerialPortManager::openPort(const SerialPortConfig& config)
{
    setPortConfig(config.portName, config);
    return openPort(config.portName);
}

void SerialPortManager::closePort(const QString& portName)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_openPorts.contains(portName)) {
        auto& port = m_openPorts[portName];
        if (port && port->isOpen()) {
            port->close();
            qDebug() << "Port closed:" << portName;
        }
        m_openPorts.erase(portName);
        
        locker.unlock();
        emit portClosed(portName);
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
    QMutexLocker locker(&m_mutex);
    auto it = m_openPorts.find(portName);
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
    QMutexLocker locker(&m_mutex);
    
    if (m_openPorts.contains(portName) && m_openPorts[portName]) {
        return m_openPorts[portName]->clear();
    }
    return false;
}

QString SerialPortManager::lastError(const QString& portName) const
{
    QMutexLocker locker(&m_mutex);
    return m_lastErrors.value(portName);
}

//=============================================================================
// Private Methods
//=============================================================================

QSerialPort* SerialPortManager::getPort(const QString& portName, bool autoOpen)
{
    QMutexLocker locker(&m_mutex);
    
    // Check if port exists and is open
    if (m_openPorts.contains(portName) && m_openPorts[portName]) {
        if (m_openPorts[portName]->isOpen()) {
            return m_openPorts[portName].get();
        }
    }
    
    // Auto-open if requested
    if (autoOpen) {
        locker.unlock();
        SerialResult result = openPort(portName);
        locker.relock();
        
        if (result.success && m_openPorts.contains(portName)) {
            return m_openPorts[portName].get();
        }
    }
    
    return nullptr;
}

bool SerialPortManager::applyConfig(QSerialPort* port, const SerialPortConfig& config)
{
    if (!port) return false;
    
    port->setBaudRate(config.baudRate);
    port->setDataBits(config.dataBits);
    port->setStopBits(config.stopBits);
    port->setParity(config.parity);
    port->setFlowControl(config.flowControl);
    
    return true;
}

} // namespace SerialManager
