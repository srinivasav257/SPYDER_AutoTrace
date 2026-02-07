#pragma once
/**
 * @file SerialManager.h
 * @brief Singleton manager for serial port communications.
 * 
 * The SerialManager provides centralized serial port management:
 * - Opens and maintains serial port connections
 * - Allows commands to use existing connections without re-opening
 * - Thread-safe access to serial ports
 * - Configurable port settings (baud rate, data bits, etc.)
 */

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QMap>
#include <map>
#include <memory>

namespace SerialManager {

//=============================================================================
// Serial Port Configuration
//=============================================================================

/**
 * @brief Configuration for a serial port
 */
struct SerialPortConfig
{
    QString portName = "COM1";      ///< Port name (COM1, /dev/ttyUSB0, etc.)
    qint32 baudRate = 115200;       ///< Baud rate
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    int readTimeoutMs = 1000;       ///< Default read timeout
    int writeTimeoutMs = 1000;      ///< Default write timeout
    
    // Conversion helpers
    static QSerialPort::DataBits dataBitsFromInt(int bits);
    static QSerialPort::StopBits stopBitsFromInt(int bits);
    static QSerialPort::Parity parityFromString(const QString& parity);
    static QSerialPort::FlowControl flowControlFromString(const QString& flow);
};

//=============================================================================
// Serial Operation Result
//=============================================================================

/**
 * @brief Result of a serial operation
 */
struct SerialResult
{
    bool success = false;           ///< Whether operation succeeded
    QString errorMessage;           ///< Error message if failed
    QByteArray data;                ///< Data received (for read operations)
    int bytesWritten = 0;           ///< Bytes written (for write operations)
    bool matchFound = false;        ///< For match operations
    
    static SerialResult Success(const QByteArray& data = {}, int written = 0) {
        return {true, QString(), data, written, false};
    }
    
    static SerialResult Failure(const QString& error) {
        return {false, error, {}, 0, false};
    }
    
    static SerialResult MatchSuccess(const QByteArray& data) {
        return {true, QString(), data, 0, true};
    }
    
    static SerialResult MatchFailure(const QString& error, const QByteArray& receivedData = {}) {
        return {false, error, receivedData, 0, false};
    }
};

//=============================================================================
// SerialPortManager Singleton
//=============================================================================

/**
 * @brief Centralized manager for serial port connections.
 * 
 * Usage:
 * @code
 * auto& serial = SerialPortManager::instance();
 * 
 * // Configure and open a port
 * SerialPortConfig config;
 * config.portName = "COM3";
 * config.baudRate = 115200;
 * serial.openPort(config);
 * 
 * // Send data (uses existing connection if open)
 * auto result = serial.send("COM3", "Hello World");
 * 
 * // Send and wait for response match
 * auto matchResult = serial.sendAndMatchResponse("COM3", "AT\r\n", "OK", 5000);
 * 
 * // Close when done
 * serial.closePort("COM3");
 * @endcode
 */
class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     */
    static SerialPortManager& instance();

    // === Port Configuration ===
    
    /**
     * @brief Set default configuration for a port
     * @param portName Port identifier
     * @param config Configuration to use
     */
    void setPortConfig(const QString& portName, const SerialPortConfig& config);
    
    /**
     * @brief Get configuration for a port
     * @param portName Port identifier
     * @return Current configuration (or default if not set)
     */
    SerialPortConfig portConfig(const QString& portName) const;
    
    /**
     * @brief Check if a port has been configured
     */
    bool hasPortConfig(const QString& portName) const;

    // === Port Management ===
    
    /**
     * @brief Open a serial port
     * @param portName Port to open (uses stored config)
     * @return Result of operation
     */
    SerialResult openPort(const QString& portName);
    
    /**
     * @brief Open a serial port with specific configuration
     * @param config Configuration to use (stores it for future use)
     * @return Result of operation
     */
    SerialResult openPort(const SerialPortConfig& config);
    
    /**
     * @brief Close a serial port
     * @param portName Port to close
     */
    void closePort(const QString& portName);
    
    /**
     * @brief Close all open ports
     */
    void closeAllPorts();
    
    /**
     * @brief Check if a port is currently open
     */
    bool isPortOpen(const QString& portName) const;
    
    /**
     * @brief Get list of currently open ports
     */
    QStringList openPorts() const;
    
    /**
     * @brief Get list of available serial ports on system
     */
    static QStringList availablePorts();

    // === Data Operations ===
    
    /**
     * @brief Send data to a serial port
     * @param portName Port to send to
     * @param data Data to send
     * @return Result with bytes written
     * 
     * If port is not open, attempts to open it using stored configuration.
     */
    SerialResult send(const QString& portName, const QByteArray& data);
    
    /**
     * @brief Send string data to a serial port
     * @param portName Port to send to
     * @param data String data to send (UTF-8 encoded)
     * @return Result with bytes written
     */
    SerialResult send(const QString& portName, const QString& data);
    
    /**
     * @brief Read data from a serial port
     * @param portName Port to read from
     * @param timeoutMs Maximum time to wait for data
     * @return Result with received data
     */
    SerialResult read(const QString& portName, int timeoutMs = -1);
    
    /**
     * @brief Read until a specific pattern is found or timeout
     * @param portName Port to read from
     * @param pattern Pattern to look for
     * @param timeoutMs Maximum time to wait
     * @return Result with received data
     */
    SerialResult readUntil(const QString& portName, const QByteArray& pattern, int timeoutMs = -1);
    
    /**
     * @brief Send data and wait for matching response
     * @param portName Port to use
     * @param sendData Data to send
     * @param expectedResponse String to find in response
     * @param timeoutMs Maximum time to wait for response
     * @return Result indicating if match was found
     * 
     * This function:
     * 1. Opens port if not already open (using stored config)
     * 2. Clears any pending input data
     * 3. Sends the data
     * 4. Reads response until timeout or match found
     * 5. Returns success if expectedResponse is found in received data
     */
    SerialResult sendAndMatchResponse(const QString& portName, 
                                       const QByteArray& sendData,
                                       const QString& expectedResponse,
                                       int timeoutMs);
    
    /**
     * @brief Send string and wait for matching response
     */
    SerialResult sendAndMatchResponse(const QString& portName,
                                       const QString& sendData,
                                       const QString& expectedResponse,
                                       int timeoutMs);

    // === Utility ===
    
    /**
     * @brief Clear input/output buffers for a port
     */
    bool clearBuffers(const QString& portName);
    
    /**
     * @brief Get last error message for a port
     */
    QString lastError(const QString& portName) const;

signals:
    /**
     * @brief Emitted when data is sent
     */
    void dataSent(const QString& portName, const QByteArray& data);
    
    /**
     * @brief Emitted when data is received
     */
    void dataReceived(const QString& portName, const QByteArray& data);
    
    /**
     * @brief Emitted when a port is opened
     */
    void portOpened(const QString& portName);
    
    /**
     * @brief Emitted when a port is closed
     */
    void portClosed(const QString& portName);
    
    /**
     * @brief Emitted on error
     */
    void errorOccurred(const QString& portName, const QString& error);

private:
    SerialPortManager();
    ~SerialPortManager() override;
    SerialPortManager(const SerialPortManager&) = delete;
    SerialPortManager& operator=(const SerialPortManager&) = delete;

    /**
     * @brief Get or create serial port object
     * @param portName Port name
     * @param autoOpen If true, open the port if not already open
     * @return Pointer to serial port (nullptr if not found and autoOpen is false)
     */
    QSerialPort* getPort(const QString& portName, bool autoOpen = true);
    
    /**
     * @brief Apply configuration to a serial port object
     * @param port Port to configure
     * @param config Configuration to apply
     * @param errorOut If non-null, receives error description on failure
     * @return true if all settings applied successfully
     */
    bool applyConfig(QSerialPort* port, const SerialPortConfig& config, QString* errorOut = nullptr);
    
    /**
     * @brief Normalize port name for consistent map lookups.
     * On Windows, converts "com3" -> "COM3", etc.
     */
    static QString normalizePortName(const QString& portName);
    
    /**
     * @brief Check if a port name exists among system-enumerated serial ports.
     */
    static bool isPortAvailableOnSystem(const QString& portName);

    mutable QMutex m_mutex;
    std::map<QString, std::unique_ptr<QSerialPort>> m_openPorts;
    QMap<QString, SerialPortConfig> m_portConfigs;
    QMap<QString, QString> m_lastErrors;
};

} // namespace SerialManager
