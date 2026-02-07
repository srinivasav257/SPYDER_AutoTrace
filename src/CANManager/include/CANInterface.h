#pragma once
/**
 * @file CANInterface.h
 * @brief Abstract CAN bus interface and common data types.
 *
 * Provides a driver-agnostic abstraction for CAN bus communication.
 * Concrete drivers (Vector XL, Kvaser, SocketCAN) implement this interface.
 */

#include <QObject>
#include <QString>
#include <QList>
#include <cstdint>

namespace CANManager {

// ============================================================================
//  CAN DLC <-> Data Length Conversion (CAN FD)
// ============================================================================

/**
 * @brief Convert CAN FD DLC code to actual data byte count.
 * Classic CAN: DLC 0–8 maps directly.
 * CAN FD: DLC 9=12, 10=16, 11=20, 12=24, 13=32, 14=48, 15=64 bytes.
 */
inline int dlcToLength(uint8_t dlc)
{
    static constexpr int table[] = {0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64};
    return (dlc <= 15) ? table[dlc] : 64;
}

/**
 * @brief Convert data byte count to CAN FD DLC code.
 * Returns the smallest DLC whose data length >= given byteCount.
 */
inline uint8_t lengthToDlc(int byteCount)
{
    if (byteCount <= 8)  return static_cast<uint8_t>(byteCount);
    if (byteCount <= 12) return 9;
    if (byteCount <= 16) return 10;
    if (byteCount <= 20) return 11;
    if (byteCount <= 24) return 12;
    if (byteCount <= 32) return 13;
    if (byteCount <= 48) return 14;
    return 15;
}

// ============================================================================
//  CAN Message
// ============================================================================

/**
 * @brief A CAN or CAN FD message (frame).
 */
struct CANMessage
{
    uint32_t id         = 0;        ///< CAN identifier (11-bit or 29-bit)
    uint8_t  data[64]   = {};       ///< Payload (up to 8 classic, up to 64 FD)
    uint8_t  dlc        = 0;        ///< Data length code
    bool     isExtended = false;    ///< 29-bit extended ID
    bool     isFD       = false;    ///< CAN FD frame (EDL flag)
    bool     isBRS      = false;    ///< Bit Rate Switch (FD only)
    bool     isRemote   = false;    ///< Remote Transmission Request
    bool     isError    = false;    ///< Error frame
    bool     isTxConfirm= false;    ///< TX confirmation (echo)
    uint64_t timestamp  = 0;        ///< Hardware timestamp (ns)

    /** Actual payload byte count (respects FD DLC mapping) */
    int dataLength() const { return isFD ? dlcToLength(dlc) : qMin((int)dlc, 8); }
};

// ============================================================================
//  CAN Channel Information (detected hardware)
// ============================================================================

/**
 * @brief Describes a detected CAN hardware channel.
 */
struct CANChannelInfo
{
    QString  name;                  ///< Channel name (e.g. "Channel 1 (VN1630)")
    QString  hwTypeName;            ///< Hardware type string (e.g. "VN1630")
    int      hwType       = 0;     ///< Hardware type code (driver-specific)
    int      hwIndex      = 0;     ///< Hardware instance index
    int      hwChannel    = 0;     ///< Channel on the hardware
    int      channelIndex = 0;     ///< Global channel index in driver config
    uint64_t channelMask  = 0;     ///< Channel access bitmask
    uint32_t serialNumber = 0;     ///< Device serial number
    bool     supportsFD   = false; ///< CAN FD capability
    bool     isOnBus      = false; ///< Currently on-bus
    QString  transceiverName;      ///< Transceiver description

    /** Display string for combo boxes: "Channel Name (S/N: 12345)" */
    QString displayString() const {
        return serialNumber > 0
            ? QString("%1  [S/N: %2]").arg(name).arg(serialNumber)
            : name;
    }
};

// ============================================================================
//  CAN Bus Configuration
// ============================================================================

/**
 * @brief Configuration for opening a CAN channel.
 */
struct CANBusConfig
{
    int  bitrate       = 500000;    ///< Nominal bitrate (bps)
    bool fdEnabled     = false;     ///< Enable CAN FD mode
    int  fdDataBitrate = 2000000;   ///< FD data-phase bitrate (bps)
    bool listenOnly    = false;     ///< Silent / listen-only mode
};

// ============================================================================
//  CAN Operation Result
// ============================================================================

/**
 * @brief Result of a CAN bus operation.
 */
struct CANResult
{
    bool    success = false;
    QString errorMessage;

    static CANResult Success() { return {true, {}}; }
    static CANResult Failure(const QString& msg) { return {false, msg}; }
};

// ============================================================================
//  ICANDriver — Abstract CAN Driver Interface
// ============================================================================

/**
 * @brief Abstract interface for CAN bus drivers.
 *
 * Each driver backend (Vector XL, Kvaser, SocketCAN, etc.) implements this
 * interface to provide a uniform CAN communication API.
 */
class ICANDriver : public QObject
{
    Q_OBJECT

public:
    explicit ICANDriver(QObject* parent = nullptr) : QObject(parent) {}
    ~ICANDriver() override = default;

    // === Driver lifecycle ===

    /** @brief Initialize the driver (load library, open driver handle). */
    virtual bool initialize() = 0;

    /** @brief Shutdown the driver (close driver handle, unload library). */
    virtual void shutdown() = 0;

    /** @brief Check if driver library is available on this system. */
    virtual bool isAvailable() const = 0;

    /** @brief Human-readable driver name (e.g. "Vector XL", "Kvaser"). */
    virtual QString driverName() const = 0;

    // === Hardware detection ===

    /** @brief Detect all available CAN channels on connected hardware. */
    virtual QList<CANChannelInfo> detectChannels() = 0;

    // === Channel management ===

    /**
     * @brief Open a CAN channel for communication.
     * @param channel  Channel info from detectChannels()
     * @param config   Bus configuration (bitrate, FD, etc.)
     */
    virtual CANResult openChannel(const CANChannelInfo& channel,
                                  const CANBusConfig& config) = 0;

    /** @brief Close the currently open channel. */
    virtual void closeChannel() = 0;

    /** @brief Check if a channel is currently open. */
    virtual bool isOpen() const = 0;

    // === Data operations ===

    /**
     * @brief Transmit a CAN message.
     * @param msg  The message to send.
     * @return Result indicating success or failure.
     */
    virtual CANResult transmit(const CANMessage& msg) = 0;

    /**
     * @brief Receive a CAN message (blocking with timeout).
     * @param[out] msg       Received message (valid only on success).
     * @param      timeoutMs Timeout in milliseconds (-1 = infinite, 0 = poll).
     * @return Result (success=true if message received, false on timeout/error).
     */
    virtual CANResult receive(CANMessage& msg, int timeoutMs = 1000) = 0;

    /** @brief Flush (discard) all messages in the receive queue. */
    virtual CANResult flushReceiveQueue() = 0;

    /** @brief Last error description from the driver backend. */
    virtual QString lastError() const = 0;

signals:
    /** @brief Emitted when a message is received (if async receive is active). */
    void messageReceived(const CANMessage& msg);

    /** @brief Emitted when an error occurs. */
    void errorOccurred(const QString& error);

    /** @brief Emitted when the channel is opened. */
    void channelOpened();

    /** @brief Emitted when the channel is closed. */
    void channelClosed();
};

} // namespace CANManager
