#pragma once
/**
 * @file VectorCANDriver.h
 * @brief Vector XL Library CAN driver implementation.
 *
 * Provides CAN bus communication through Vector hardware (VN1630, VN1610, etc.)
 * using the Vector XL Driver Library with runtime DLL loading.
 *
 * Features:
 *   - Runtime DLL loading (app works without Vector drivers installed)
 *   - Automatic hardware detection and channel enumeration
 *   - Classic CAN (HS) and CAN FD support
 *   - Thread-safe transmit/receive with notification events
 */

#include "CANInterface.h"

#include <QLibrary>
#include <QMutex>
#include <QThread>
#include <atomic>

// Use dynamic loading mode — gives us typedefs for function pointers
#define DYNAMIC_XLDRIVER_DLL
#define DO_NOT_DEFINE_EXTERN_DECLARATION
#include "vxlapi.h"

namespace CANManager {

/**
 * @brief CAN driver implementation using the Vector XL Library.
 *
 * Usage:
 * @code
 *   auto driver = new VectorCANDriver;
 *   if (driver->initialize()) {
 *       auto channels = driver->detectChannels();
 *       if (!channels.isEmpty()) {
 *           CANBusConfig cfg;
 *           cfg.bitrate = 500000;
 *           auto result = driver->openChannel(channels[0], cfg);
 *           if (result.success) {
 *               CANMessage tx;
 *               tx.id = 0x100;
 *               tx.dlc = 8;
 *               memset(tx.data, 0xAA, 8);
 *               driver->transmit(tx);
 *           }
 *       }
 *   }
 * @endcode
 */
class VectorCANDriver : public ICANDriver
{
    Q_OBJECT

public:
    explicit VectorCANDriver(QObject* parent = nullptr);
    ~VectorCANDriver() override;

    // === ICANDriver interface ===
    bool    initialize() override;
    void    shutdown() override;
    bool    isAvailable() const override;
    QString driverName() const override { return QStringLiteral("Vector XL"); }

    QList<CANChannelInfo> detectChannels() override;

    CANResult openChannel(const CANChannelInfo& channel,
                          const CANBusConfig& config) override;
    void      closeChannel() override;
    bool      isOpen() const override;

    CANResult transmit(const CANMessage& msg) override;
    CANResult receive(CANMessage& msg, int timeoutMs = 1000) override;
    CANResult flushReceiveQueue() override;

    QString   lastError() const override;

    // === Vector-specific extras ===

    /** @brief Get the XL Library DLL version string (e.g. "20.30.14"). */
    QString xlDllVersion() const;

    /** @brief Get hardware type name from type code. */
    static QString hwTypeName(int hwType);

    /** @brief Application name used in Vector Hardware Config. */
    void setAppName(const QString& appName) { m_appName = appName; }
    QString appName() const { return m_appName; }

    // === Async receive ===

    /** @brief Start a background thread that continuously receives and emits messageReceived(). */
    void startAsyncReceive();

    /** @brief Stop the async receive thread. */
    void stopAsyncReceive();

    /** @brief Check if async receive is running. */
    bool isAsyncReceiving() const { return m_asyncRunning.load(); }

private:
    // --- DLL loading ---
    bool loadLibrary();
    void unloadLibrary();
    bool resolveFunctions();

    // --- Helpers ---
    QString xlStatusToString(XLstatus status) const;
    void    setError(const QString& msg);
    CANResult makeError(const QString& context, XLstatus status);

    // Classic CAN transmit/receive
    CANResult transmitClassic(const CANMessage& msg);
    CANResult receiveClassic(CANMessage& msg, int timeoutMs);

    // CAN FD transmit/receive
    CANResult transmitFD(const CANMessage& msg);
    CANResult receiveFD(CANMessage& msg, int timeoutMs);

    // --- State ---
    QLibrary     m_xlLib;
    bool         m_driverOpen   = false;
    XLportHandle m_portHandle   = XL_INVALID_PORTHANDLE;
    XLaccess     m_channelMask  = 0;
    XLaccess     m_permissionMask = 0;
    XLhandle     m_notifyEvent  = nullptr;
    bool         m_isFD         = false;
    QString      m_lastError;
    QString      m_appName      = QStringLiteral("SPYDER_AutoTrace");
    mutable QMutex m_mutex;

    /// Cached result of isAvailable(): -1 = not yet checked, 0 = false, 1 = true
    mutable int  m_availableCached = -1;

    // --- Async receive thread ---
    QThread*           m_rxThread   = nullptr;
    std::atomic<bool>  m_asyncRunning{false};

    // --- XL Library function pointers ---
    XLOPENDRIVER                m_xlOpenDriver              = nullptr;
    XLCLOSEDRIVER               m_xlCloseDriver             = nullptr;
    XLGETDRIVERCONFIG           m_xlGetDriverConfig         = nullptr;
    XLGETAPPLCONFIG             m_xlGetApplConfig           = nullptr;
    XLSETAPPLCONFIG             m_xlSetApplConfig           = nullptr;
    XLGETCHANNELINDEX           m_xlGetChannelIndex         = nullptr;
    XLGETCHANNELMASK            m_xlGetChannelMask          = nullptr;
    XLOPENPORT                  m_xlOpenPort                = nullptr;
    XLCLOSEPORT                 m_xlClosePort               = nullptr;
    XLACTIVATECHANNEL           m_xlActivateChannel         = nullptr;
    XLDEACTIVATECHANNEL         m_xlDeactivateChannel       = nullptr;
    XLCANSETCHANNELBITRATE      m_xlCanSetChannelBitrate    = nullptr;
    XLCANSETCHANNELOUTPUT       m_xlCanSetChannelOutput     = nullptr;
    XLCANSETCHANNELMODE         m_xlCanSetChannelMode       = nullptr;
    XLCANFDSETCONFIGURATION     m_xlCanFdSetConfiguration   = nullptr;
    XLCANTRANSMIT               m_xlCanTransmit             = nullptr;
    XLCANTRANSMITEX             m_xlCanTransmitEx           = nullptr;
    XLRECEIVE                   m_xlReceive                 = nullptr;
    XLCANRECEIVE                m_xlCanReceive              = nullptr;
    XLSETNOTIFICATION           m_xlSetNotification         = nullptr;
    XLFLUSHRECEIVEQUEUE         m_xlFlushReceiveQueue       = nullptr;
    XLGETERRORSTRING            m_xlGetErrorString          = nullptr;
    XLGETEVENTSTRING            m_xlGetEventString          = nullptr;
};

} // namespace CANManager
