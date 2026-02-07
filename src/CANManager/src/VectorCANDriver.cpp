/**
 * @file VectorCANDriver.cpp
 * @brief Vector XL Library CAN driver — full implementation.
 *
 * Runtime-loads vxlapi64.dll / vxlapi.dll and provides complete CAN HS + FD
 * communication through Vector hardware.
 */

#include "VectorCANDriver.h"

#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

namespace CANManager {

// ============================================================================
//  Hardware type name lookup
// ============================================================================

QString VectorCANDriver::hwTypeName(int hwType)
{
    switch (hwType) {
    case XL_HWTYPE_VIRTUAL:           return QStringLiteral("Virtual");
    case XL_HWTYPE_CANCARDX:          return QStringLiteral("CANcardX");
    case XL_HWTYPE_CANAC2PCI:         return QStringLiteral("CANac2PCI");
    case XL_HWTYPE_CANCASEXL:         return QStringLiteral("CANcaseXL");
    case XL_HWTYPE_CANBOARDXL:        return QStringLiteral("CANboardXL");
    case XL_HWTYPE_VN1610:            return QStringLiteral("VN1610");
    case XL_HWTYPE_VN1630:            return QStringLiteral("VN1630");
    case XL_HWTYPE_VN1640:            return QStringLiteral("VN1640");
    case XL_HWTYPE_VN8900:            return QStringLiteral("VN8900");
    case XL_HWTYPE_VN7600:            return QStringLiteral("VN7600");
    case XL_HWTYPE_VN5610:            return QStringLiteral("VN5610");
    case XL_HWTYPE_VN5620:            return QStringLiteral("VN5620");
    case XL_HWTYPE_VN7610:            return QStringLiteral("VN7610");
    case XL_HWTYPE_VN7572:            return QStringLiteral("VN7572");
    case XL_HWTYPE_VN1530:            return QStringLiteral("VN1530");
    case XL_HWTYPE_VN1531:            return QStringLiteral("VN1531");
    case XL_HWTYPE_VN1670:            return QStringLiteral("VN1670");
    case XL_HWTYPE_VN5610A:           return QStringLiteral("VN5610A");
    case XL_HWTYPE_VN5620A:           return QStringLiteral("VN5620A");
    case XL_HWTYPE_VN7640:            return QStringLiteral("VN7640");
    case XL_HWTYPE_VN4610:            return QStringLiteral("VN4610");
    default:
        return QString("HW_Type_%1").arg(hwType);
    }
}

// ============================================================================
//  Constructor / Destructor
// ============================================================================

VectorCANDriver::VectorCANDriver(QObject* parent)
    : ICANDriver(parent)
{
}

VectorCANDriver::~VectorCANDriver()
{
    shutdown();
}

// ============================================================================
//  DLL Loading
// ============================================================================

bool VectorCANDriver::loadLibrary()
{
    if (m_xlLib.isLoaded())
        return true;

    // Try 64-bit DLL first (common on modern systems), then 32-bit
    static const QStringList dllNames = {
        QStringLiteral("vxlapi64"),
        QStringLiteral("vxlapi")
    };

    for (const auto& name : dllNames) {
        m_xlLib.setFileName(name);
        if (m_xlLib.load()) {
            qDebug() << "[VectorCAN] Loaded" << m_xlLib.fileName();
            return true;
        }
    }

    // Try from third_party folder
    QString appDir = QCoreApplication::applicationDirPath();
    for (const auto& name : dllNames) {
        m_xlLib.setFileName(appDir + "/../../third_party/vector_xl/bin/" + name);
        if (m_xlLib.load()) {
            qDebug() << "[VectorCAN] Loaded from third_party:" << m_xlLib.fileName();
            return true;
        }
    }

    setError(QString("Failed to load Vector XL Library: %1").arg(m_xlLib.errorString()));
    return false;
}

void VectorCANDriver::unloadLibrary()
{
    if (m_xlLib.isLoaded()) {
        m_xlLib.unload();
        qDebug() << "[VectorCAN] Library unloaded";
    }

    // Clear all function pointers
    m_xlOpenDriver = nullptr;
    m_xlCloseDriver = nullptr;
    m_xlGetDriverConfig = nullptr;
    m_xlGetApplConfig = nullptr;
    m_xlSetApplConfig = nullptr;
    m_xlGetChannelIndex = nullptr;
    m_xlGetChannelMask = nullptr;
    m_xlOpenPort = nullptr;
    m_xlClosePort = nullptr;
    m_xlActivateChannel = nullptr;
    m_xlDeactivateChannel = nullptr;
    m_xlCanSetChannelBitrate = nullptr;
    m_xlCanSetChannelOutput = nullptr;
    m_xlCanSetChannelMode = nullptr;
    m_xlCanFdSetConfiguration = nullptr;
    m_xlCanTransmit = nullptr;
    m_xlCanTransmitEx = nullptr;
    m_xlReceive = nullptr;
    m_xlCanReceive = nullptr;
    m_xlSetNotification = nullptr;
    m_xlFlushReceiveQueue = nullptr;
    m_xlGetErrorString = nullptr;
    m_xlGetEventString = nullptr;
}

bool VectorCANDriver::resolveFunctions()
{
    if (!m_xlLib.isLoaded())
        return false;

    // Helper macro for resolving functions
    #define RESOLVE_XL(funcName, typeName) \
        m_##funcName = reinterpret_cast<typeName>(m_xlLib.resolve(#funcName)); \
        if (!m_##funcName) { \
            qWarning() << "[VectorCAN] Failed to resolve:" << #funcName; \
            return false; \
        }

    RESOLVE_XL(xlOpenDriver,            XLOPENDRIVER)
    RESOLVE_XL(xlCloseDriver,           XLCLOSEDRIVER)
    RESOLVE_XL(xlGetDriverConfig,       XLGETDRIVERCONFIG)
    RESOLVE_XL(xlOpenPort,              XLOPENPORT)
    RESOLVE_XL(xlClosePort,             XLCLOSEPORT)
    RESOLVE_XL(xlActivateChannel,       XLACTIVATECHANNEL)
    RESOLVE_XL(xlDeactivateChannel,     XLDEACTIVATECHANNEL)
    RESOLVE_XL(xlCanSetChannelBitrate,  XLCANSETCHANNELBITRATE)
    RESOLVE_XL(xlCanSetChannelOutput,   XLCANSETCHANNELOUTPUT)
    RESOLVE_XL(xlSetNotification,       XLSETNOTIFICATION)
    RESOLVE_XL(xlFlushReceiveQueue,     XLFLUSHRECEIVEQUEUE)
    RESOLVE_XL(xlCanTransmit,           XLCANTRANSMIT)
    RESOLVE_XL(xlReceive,               XLRECEIVE)

    // Optional functions (may not exist in older DLL versions)
    #define RESOLVE_XL_OPTIONAL(funcName, typeName) \
        m_##funcName = reinterpret_cast<typeName>(m_xlLib.resolve(#funcName)); \
        if (!m_##funcName) { \
            qDebug() << "[VectorCAN] Optional function not found:" << #funcName; \
        }

    RESOLVE_XL_OPTIONAL(xlGetApplConfig,          XLGETAPPLCONFIG)
    RESOLVE_XL_OPTIONAL(xlSetApplConfig,           XLSETAPPLCONFIG)
    RESOLVE_XL_OPTIONAL(xlGetChannelIndex,         XLGETCHANNELINDEX)
    RESOLVE_XL_OPTIONAL(xlGetChannelMask,          XLGETCHANNELMASK)
    RESOLVE_XL_OPTIONAL(xlCanSetChannelMode,       XLCANSETCHANNELMODE)
    RESOLVE_XL_OPTIONAL(xlCanFdSetConfiguration,   XLCANFDSETCONFIGURATION)
    RESOLVE_XL_OPTIONAL(xlCanTransmitEx,           XLCANTRANSMITEX)
    RESOLVE_XL_OPTIONAL(xlCanReceive,              XLCANRECEIVE)
    RESOLVE_XL_OPTIONAL(xlGetErrorString,          XLGETERRORSTRING)
    RESOLVE_XL_OPTIONAL(xlGetEventString,          XLGETEVENTSTRING)

    #undef RESOLVE_XL
    #undef RESOLVE_XL_OPTIONAL

    return true;
}

// ============================================================================
//  ICANDriver — Lifecycle
// ============================================================================

bool VectorCANDriver::initialize()
{
    QMutexLocker locker(&m_mutex);

    if (m_driverOpen)
        return true;

    if (!loadLibrary())
        return false;

    if (!resolveFunctions()) {
        unloadLibrary();
        return false;
    }

    XLstatus status = m_xlOpenDriver();
    if (status != XL_SUCCESS) {
        setError(QString("xlOpenDriver failed: %1").arg(xlStatusToString(status)));
        unloadLibrary();
        return false;
    }

    m_driverOpen = true;
    qDebug() << "[VectorCAN] Driver initialized. DLL version:" << xlDllVersion();
    return true;
}

void VectorCANDriver::shutdown()
{
    // Stop async receive before anything else
    stopAsyncReceive();

    QMutexLocker locker(&m_mutex);

    if (m_portHandle != XL_INVALID_PORTHANDLE) {
        locker.unlock();
        closeChannel();
        locker.relock();
    }

    if (m_driverOpen && m_xlCloseDriver) {
        m_xlCloseDriver();
        m_driverOpen = false;
        qDebug() << "[VectorCAN] Driver closed";
    }

    unloadLibrary();
}

bool VectorCANDriver::isAvailable() const
{
    if (m_availableCached >= 0)
        return m_availableCached == 1;

    // Quick check: try loading the library without keeping it
    QLibrary testLib;
    for (const auto& name : {QStringLiteral("vxlapi64"), QStringLiteral("vxlapi")}) {
        testLib.setFileName(name);
        if (testLib.load()) {
            testLib.unload();
            m_availableCached = 1;
            return true;
        }
    }
    m_availableCached = 0;
    return false;
}

QString VectorCANDriver::xlDllVersion() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_driverOpen || !m_xlGetDriverConfig)
        return {};

    XLdriverConfig drvConfig;
    memset(&drvConfig, 0, sizeof(drvConfig));
    XLstatus status = m_xlGetDriverConfig(&drvConfig);
    if (status != XL_SUCCESS)
        return {};

    unsigned int ver = drvConfig.dllVersion;
    return QString("%1.%2.%3")
        .arg((ver >> 24) & 0xFF)
        .arg((ver >> 16) & 0xFF)
        .arg(ver & 0xFFFF);
}

// ============================================================================
//  Hardware Detection
// ============================================================================

QList<CANChannelInfo> VectorCANDriver::detectChannels()
{
    QMutexLocker locker(&m_mutex);
    QList<CANChannelInfo> channels;

    if (!m_driverOpen || !m_xlGetDriverConfig) {
        setError("Driver not initialized");
        return channels;
    }

    XLdriverConfig drvConfig;
    memset(&drvConfig, 0, sizeof(drvConfig));
    XLstatus status = m_xlGetDriverConfig(&drvConfig);
    if (status != XL_SUCCESS) {
        setError(QString("xlGetDriverConfig failed: %1").arg(xlStatusToString(status)));
        return channels;
    }

    qDebug() << "[VectorCAN] Detected" << drvConfig.channelCount << "channels";

    for (unsigned int i = 0; i < drvConfig.channelCount && i < XL_CONFIG_MAX_CHANNELS; ++i) {
        const auto& ch = drvConfig.channel[i];

        // Only include channels that support CAN bus
        if (!(ch.channelBusCapabilities & XL_BUS_COMPATIBLE_CAN))
            continue;

        CANChannelInfo info;
        info.name            = QString::fromLatin1(ch.name);
        info.hwTypeName      = hwTypeName(ch.hwType);
        info.hwType          = ch.hwType;
        info.hwIndex         = ch.hwIndex;
        info.hwChannel       = ch.hwChannel;
        info.channelIndex    = ch.channelIndex;
        info.channelMask     = ch.channelMask;
        info.serialNumber    = ch.serialNumber;
        info.isOnBus         = ch.isOnBus != 0;
        info.transceiverName = QString::fromLatin1(ch.transceiverName);

        // Check CAN FD support via channel capabilities
        info.supportsFD = (ch.channelCapabilities & XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT) ||
                          (ch.channelCapabilities & XL_CHANNEL_FLAG_CANFD_BOSCH_SUPPORT);

        qDebug() << "[VectorCAN]  Ch" << i << ":" << info.name
                 << "HW:" << info.hwTypeName
                 << "S/N:" << info.serialNumber
                 << "FD:" << info.supportsFD
                 << "Mask:" << Qt::hex << info.channelMask;

        channels.append(info);
    }

    return channels;
}

// ============================================================================
//  Channel Open / Close
// ============================================================================

CANResult VectorCANDriver::openChannel(const CANChannelInfo& channel,
                                        const CANBusConfig& config)
{
    QMutexLocker locker(&m_mutex);

    if (!m_driverOpen)
        return CANResult::Failure("Driver not initialized");

    if (m_portHandle != XL_INVALID_PORTHANDLE)
        return CANResult::Failure("A channel is already open — close it first");

    m_isFD = config.fdEnabled && channel.supportsFD;
    m_channelMask = channel.channelMask;
    m_permissionMask = channel.channelMask;  // Request init access

    // Choose interface version based on FD mode
    unsigned int interfaceVersion = m_isFD ? XL_INTERFACE_VERSION_V4
                                           : XL_INTERFACE_VERSION;

    // Open port
    QByteArray appNameUtf8 = m_appName.toUtf8();
    XLstatus status = m_xlOpenPort(
        &m_portHandle,
        appNameUtf8.data(),
        m_channelMask,
        &m_permissionMask,
        256,                    // RX queue size (events)
        interfaceVersion,
        XL_BUS_TYPE_CAN
    );

    if (status != XL_SUCCESS) {
        m_portHandle = XL_INVALID_PORTHANDLE;
        return makeError("xlOpenPort", status);
    }

    qDebug() << "[VectorCAN] Port opened. Handle:" << m_portHandle
             << "PermMask:" << Qt::hex << m_permissionMask;

    // Check if we got init (transmit) access
    if (!(m_permissionMask & m_channelMask)) {
        qWarning() << "[VectorCAN] No init access — channel may be used by another app. "
                       "Will be in listen-only mode.";
    }

    // Configure the channel
    if (m_permissionMask & m_channelMask) {
        if (m_isFD) {
            // CAN FD configuration
            if (m_xlCanFdSetConfiguration) {
                XLcanFdConf fdConf;
                memset(&fdConf, 0, sizeof(fdConf));
                fdConf.arbitrationBitRate = static_cast<unsigned int>(config.bitrate);
                fdConf.dataBitRate        = static_cast<unsigned int>(config.fdDataBitrate);

                // Use sensible default timing parameters
                // The XL library will calculate optimal timing from bitrates
                // when tseg values are set to 0
                fdConf.sjwAbr   = 0;
                fdConf.tseg1Abr = 0;
                fdConf.tseg2Abr = 0;
                fdConf.sjwDbr   = 0;
                fdConf.tseg1Dbr = 0;
                fdConf.tseg2Dbr = 0;

                status = m_xlCanFdSetConfiguration(m_portHandle, m_channelMask, &fdConf);
                if (status != XL_SUCCESS) {
                    qWarning() << "[VectorCAN] xlCanFdSetConfiguration failed:"
                               << xlStatusToString(status)
                               << "- falling back to classic CAN";
                    m_isFD = false;
                    // Try classic bitrate instead
                    status = m_xlCanSetChannelBitrate(m_portHandle, m_channelMask,
                                                      static_cast<unsigned long>(config.bitrate));
                    if (status != XL_SUCCESS) {
                        qWarning() << "[VectorCAN] xlCanSetChannelBitrate also failed:"
                                   << xlStatusToString(status);
                    }
                }
            } else {
                qWarning() << "[VectorCAN] FD requested but xlCanFdSetConfiguration not available";
                m_isFD = false;
            }
        }

        if (!m_isFD) {
            // Classic CAN bitrate configuration
            status = m_xlCanSetChannelBitrate(m_portHandle, m_channelMask,
                                              static_cast<unsigned long>(config.bitrate));
            if (status != XL_SUCCESS) {
                qWarning() << "[VectorCAN] xlCanSetChannelBitrate warning:"
                           << xlStatusToString(status);
                // Non-fatal: channel might already have this bitrate set
            }
        }

        // Set normal output mode (unless listen-only requested)
        int outputMode = config.listenOnly ? XL_OUTPUT_MODE_SILENT : XL_OUTPUT_MODE_NORMAL;
        m_xlCanSetChannelOutput(m_portHandle, m_channelMask, outputMode);
    }

    // Set up notification event for receive
    m_notifyEvent = nullptr;
    status = m_xlSetNotification(m_portHandle, &m_notifyEvent, 1);
    if (status != XL_SUCCESS) {
        qWarning() << "[VectorCAN] xlSetNotification warning:" << xlStatusToString(status);
        // Non-fatal: receive with polling will still work (less efficient)
    }

    // Activate channel (go on-bus)
    status = m_xlActivateChannel(m_portHandle, m_channelMask,
                                  XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
    if (status != XL_SUCCESS) {
        auto err = makeError("xlActivateChannel", status);
        // Clean up
        m_xlClosePort(m_portHandle);
        m_portHandle = XL_INVALID_PORTHANDLE;
        m_notifyEvent = nullptr;
        return err;
    }

    // Flush any stale messages
    m_xlFlushReceiveQueue(m_portHandle);

    qDebug() << "[VectorCAN] Channel activated."
             << "FD:" << m_isFD
             << "Bitrate:" << config.bitrate
             << (m_isFD ? QString(" FD Data BR: %1").arg(config.fdDataBitrate) : QString());

    locker.unlock();
    emit channelOpened();

    return CANResult::Success();
}

void VectorCANDriver::closeChannel()
{
    // Stop async receive first
    stopAsyncReceive();

    QMutexLocker locker(&m_mutex);

    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return;

    // Deactivate channel (go off-bus)
    if (m_xlDeactivateChannel)
        m_xlDeactivateChannel(m_portHandle, m_channelMask);

    // Close port
    if (m_xlClosePort)
        m_xlClosePort(m_portHandle);

    qDebug() << "[VectorCAN] Channel closed. Handle was:" << m_portHandle;

    m_portHandle = XL_INVALID_PORTHANDLE;
    m_channelMask = 0;
    m_permissionMask = 0;
    m_notifyEvent = nullptr;
    m_isFD = false;

    locker.unlock();
    emit channelClosed();
}

bool VectorCANDriver::isOpen() const
{
    QMutexLocker locker(&m_mutex);
    return m_portHandle != XL_INVALID_PORTHANDLE;
}

// ============================================================================
//  Transmit
// ============================================================================

CANResult VectorCANDriver::transmit(const CANMessage& msg)
{
    QMutexLocker locker(&m_mutex);

    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return CANResult::Failure("Channel not open");

    if (!(m_permissionMask & m_channelMask))
        return CANResult::Failure("No transmit access (channel opened by another application)");

    if (msg.isFD && m_isFD) {
        return transmitFD(msg);
    }
    return transmitClassic(msg);
}

CANResult VectorCANDriver::transmitClassic(const CANMessage& msg)
{
    // Build classic CAN event
    XLevent xlEvent;
    memset(&xlEvent, 0, sizeof(xlEvent));

    xlEvent.tag = XL_TRANSMIT_MSG;
    xlEvent.tagData.msg.id    = msg.id;
    xlEvent.tagData.msg.dlc   = qMin((unsigned short)msg.dlc, (unsigned short)8);
    xlEvent.tagData.msg.flags = 0;

    if (msg.isExtended)
        xlEvent.tagData.msg.id |= XL_CAN_EXT_MSG_ID;

    if (msg.isRemote)
        xlEvent.tagData.msg.flags |= XL_CAN_MSG_FLAG_REMOTE_FRAME;

    memcpy(xlEvent.tagData.msg.data, msg.data, qMin((int)msg.dlc, 8));

    unsigned int msgCount = 1;
    XLstatus status = m_xlCanTransmit(m_portHandle, m_channelMask, &msgCount, &xlEvent);

    if (status != XL_SUCCESS)
        return makeError("xlCanTransmit", status);

    return CANResult::Success();
}

CANResult VectorCANDriver::transmitFD(const CANMessage& msg)
{
    if (!m_xlCanTransmitEx)
        return CANResult::Failure("CAN FD transmit not available (xlCanTransmitEx missing)");

    XLcanTxEvent txEvent;
    memset(&txEvent, 0, sizeof(txEvent));

    txEvent.tag = XL_CAN_EV_TAG_TX_MSG;
    txEvent.transId = 0;
    txEvent.channelIndex = 0;  // internal, must be 0

    txEvent.tagData.canMsg.canId = msg.id;
    if (msg.isExtended)
        txEvent.tagData.canMsg.canId |= XL_CAN_EXT_MSG_ID;

    txEvent.tagData.canMsg.msgFlags = 0;
    if (msg.isFD)
        txEvent.tagData.canMsg.msgFlags |= XL_CAN_TXMSG_FLAG_EDL;
    if (msg.isBRS)
        txEvent.tagData.canMsg.msgFlags |= XL_CAN_TXMSG_FLAG_BRS;
    if (msg.isRemote)
        txEvent.tagData.canMsg.msgFlags |= XL_CAN_TXMSG_FLAG_RTR;

    txEvent.tagData.canMsg.dlc = msg.dlc;

    int dataLen = msg.isFD ? dlcToLength(msg.dlc) : qMin((int)msg.dlc, 8);
    memcpy(txEvent.tagData.canMsg.data, msg.data, dataLen);

    unsigned int msgCntSent = 0;
    XLstatus status = m_xlCanTransmitEx(m_portHandle, m_channelMask,
                                         1, &msgCntSent, &txEvent);

    if (status != XL_SUCCESS)
        return makeError("xlCanTransmitEx", status);

    if (msgCntSent == 0)
        return CANResult::Failure("Message was not sent (queue full?)");

    return CANResult::Success();
}

// ============================================================================
//  Receive
// ============================================================================

CANResult VectorCANDriver::receive(CANMessage& msg, int timeoutMs)
{
    QMutexLocker locker(&m_mutex);

    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return CANResult::Failure("Channel not open");

    if (m_isFD && m_xlCanReceive)
        return receiveFD(msg, timeoutMs);

    return receiveClassic(msg, timeoutMs);
}

CANResult VectorCANDriver::receiveClassic(CANMessage& msg, int timeoutMs)
{
#ifdef _WIN32
    // Wait for notification event
    if (m_notifyEvent) {
        DWORD waitMs = (timeoutMs < 0) ? INFINITE : static_cast<DWORD>(timeoutMs);
        DWORD waitResult = WaitForSingleObject(m_notifyEvent, waitMs);
        if (waitResult == WAIT_TIMEOUT)
            return CANResult::Failure("Receive timeout");
        if (waitResult != WAIT_OBJECT_0)
            return CANResult::Failure("Wait error");
    }
#else
    Q_UNUSED(timeoutMs);
#endif

    XLevent xlEvent;
    unsigned int eventCount = 1;
    XLstatus status = m_xlReceive(m_portHandle, &eventCount, &xlEvent);

    if (status == XL_ERR_QUEUE_IS_EMPTY)
        return CANResult::Failure("No message available");

    if (status != XL_SUCCESS)
        return makeError("xlReceive", status);

    // Parse only CAN receive messages
    if (xlEvent.tag != XL_RECEIVE_MSG)
        return CANResult::Failure("Non-CAN event received");

    // Skip TX confirmation echoes unless flagged
    if (xlEvent.tagData.msg.flags & XL_CAN_MSG_FLAG_TX_COMPLETED) {
        msg.isTxConfirm = true;
    }

    msg.id         = xlEvent.tagData.msg.id & ~XL_CAN_EXT_MSG_ID;
    msg.isExtended = (xlEvent.tagData.msg.id & XL_CAN_EXT_MSG_ID) != 0;
    msg.dlc        = static_cast<uint8_t>(qMin((unsigned short)xlEvent.tagData.msg.dlc,
                                                (unsigned short)8));
    msg.isFD       = false;
    msg.isBRS      = false;
    msg.isRemote   = (xlEvent.tagData.msg.flags & XL_CAN_MSG_FLAG_REMOTE_FRAME) != 0;
    msg.isError    = (xlEvent.tagData.msg.flags & XL_CAN_MSG_FLAG_ERROR_FRAME) != 0;
    msg.timestamp  = xlEvent.timeStamp;

    memcpy(msg.data, xlEvent.tagData.msg.data, msg.dlc);

    return CANResult::Success();
}

CANResult VectorCANDriver::receiveFD(CANMessage& msg, int timeoutMs)
{
#ifdef _WIN32
    // Wait for notification event
    if (m_notifyEvent) {
        DWORD waitMs = (timeoutMs < 0) ? INFINITE : static_cast<DWORD>(timeoutMs);
        DWORD waitResult = WaitForSingleObject(m_notifyEvent, waitMs);
        if (waitResult == WAIT_TIMEOUT)
            return CANResult::Failure("Receive timeout");
        if (waitResult != WAIT_OBJECT_0)
            return CANResult::Failure("Wait error");
    }
#else
    Q_UNUSED(timeoutMs);
#endif

    XLcanRxEvent rxEvent;
    memset(&rxEvent, 0, sizeof(rxEvent));
    XLstatus status = m_xlCanReceive(m_portHandle, &rxEvent);

    if (status == XL_ERR_QUEUE_IS_EMPTY)
        return CANResult::Failure("No message available");

    if (status != XL_SUCCESS)
        return makeError("xlCanReceive", status);

    // Handle different event tags
    switch (rxEvent.tag) {
    case XL_CAN_EV_TAG_RX_OK:
    case XL_CAN_EV_TAG_TX_OK:
    {
        const auto& rxMsg = rxEvent.tagData.canRxOkMsg;

        msg.id         = rxMsg.canId & ~XL_CAN_EXT_MSG_ID;
        msg.isExtended = (rxMsg.canId & XL_CAN_EXT_MSG_ID) != 0;
        msg.dlc        = rxMsg.dlc;
        msg.isFD       = (rxMsg.msgFlags & XL_CAN_RXMSG_FLAG_EDL) != 0;
        msg.isBRS      = (rxMsg.msgFlags & XL_CAN_RXMSG_FLAG_BRS) != 0;
        msg.isRemote   = (rxMsg.msgFlags & XL_CAN_RXMSG_FLAG_RTR) != 0;
        msg.isError    = (rxMsg.msgFlags & XL_CAN_RXMSG_FLAG_EF) != 0;
        msg.isTxConfirm = (rxEvent.tag == XL_CAN_EV_TAG_TX_OK);
        msg.timestamp  = rxEvent.timeStampSync;

        int dataLen = msg.isFD ? dlcToLength(msg.dlc) : qMin((int)msg.dlc, 8);
        memcpy(msg.data, rxMsg.data, dataLen);

        return CANResult::Success();
    }

    case XL_CAN_EV_TAG_CHIP_STATE:
        return CANResult::Failure("Chip state event (not a message)");

    case XL_CAN_EV_TAG_TX_ERROR:
    case XL_CAN_EV_TAG_RX_ERROR:
        return CANResult::Failure("CAN error event received");

    default:
        return CANResult::Failure(QString("Unknown FD event tag: 0x%1")
                                      .arg(rxEvent.tag, 4, 16, QChar('0')));
    }
}

// ============================================================================
//  Async Receive Thread
// ============================================================================

void VectorCANDriver::startAsyncReceive()
{
    if (m_asyncRunning.load())
        return;

    if (!isOpen()) {
        qWarning() << "[VectorCAN] Cannot start async receive \u2014 channel not open";
        return;
    }

    m_asyncRunning = true;
    m_rxThread = QThread::create([this]() {
        qDebug() << "[VectorCAN] Async receive thread started";
        while (m_asyncRunning.load()) {
            CANMessage msg;
            CANResult result = receive(msg, 100);  // 100ms poll
            if (result.success && !msg.isError) {
                emit messageReceived(msg);
            }
            // On timeout or empty queue, just loop
        }
        qDebug() << "[VectorCAN] Async receive thread stopped";
    });
    m_rxThread->setObjectName(QStringLiteral("VectorCAN_RxThread"));
    m_rxThread->start();
}

void VectorCANDriver::stopAsyncReceive()
{
    if (!m_asyncRunning.load())
        return;

    m_asyncRunning = false;
    if (m_rxThread) {
        m_rxThread->wait(3000);
        delete m_rxThread;
        m_rxThread = nullptr;
    }
}

// ============================================================================
//  Flush
// ============================================================================

CANResult VectorCANDriver::flushReceiveQueue()
{
    QMutexLocker locker(&m_mutex);

    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return CANResult::Failure("Channel not open");

    XLstatus status = m_xlFlushReceiveQueue(m_portHandle);
    if (status != XL_SUCCESS)
        return makeError("xlFlushReceiveQueue", status);

    return CANResult::Success();
}

// ============================================================================
//  Error Handling Helpers
// ============================================================================

QString VectorCANDriver::lastError() const
{
    QMutexLocker locker(&m_mutex);
    return m_lastError;
}

QString VectorCANDriver::xlStatusToString(XLstatus status) const
{
    if (m_xlGetErrorString) {
        const char* str = m_xlGetErrorString(status);
        if (str)
            return QString::fromLatin1(str);
    }

    // Fallback: numeric code
    switch (status) {
    case XL_SUCCESS:              return QStringLiteral("XL_SUCCESS");
    case XL_ERR_QUEUE_IS_EMPTY:   return QStringLiteral("XL_ERR_QUEUE_IS_EMPTY");
    case XL_ERR_QUEUE_IS_FULL:    return QStringLiteral("XL_ERR_QUEUE_IS_FULL");
    case XL_ERR_TX_NOT_POSSIBLE:  return QStringLiteral("XL_ERR_TX_NOT_POSSIBLE");
    case XL_ERR_NO_LICENSE:       return QStringLiteral("XL_ERR_NO_LICENSE");
    case XL_ERR_WRONG_PARAMETER:  return QStringLiteral("XL_ERR_WRONG_PARAMETER");
    case XL_ERR_INVALID_CHAN_INDEX:return QStringLiteral("XL_ERR_INVALID_CHAN_INDEX");
    case XL_ERR_INVALID_ACCESS:   return QStringLiteral("XL_ERR_INVALID_ACCESS");
    case XL_ERR_PORT_IS_OFFLINE:  return QStringLiteral("XL_ERR_PORT_IS_OFFLINE");
    case XL_ERR_HW_NOT_PRESENT:   return QStringLiteral("XL_ERR_HW_NOT_PRESENT");
    case XL_ERR_CANNOT_OPEN_DRIVER: return QStringLiteral("XL_ERR_CANNOT_OPEN_DRIVER");
    case XL_ERR_WRONG_BUS_TYPE:   return QStringLiteral("XL_ERR_WRONG_BUS_TYPE");
    case XL_ERR_DLL_NOT_FOUND:    return QStringLiteral("XL_ERR_DLL_NOT_FOUND");
    default:
        return QString("XL_ERR_%1 (0x%2)").arg(status).arg((unsigned short)status, 4, 16, QChar('0'));
    }
}

void VectorCANDriver::setError(const QString& msg)
{
    m_lastError = msg;
    qWarning() << "[VectorCAN]" << msg;
}

CANResult VectorCANDriver::makeError(const QString& context, XLstatus status)
{
    QString msg = QString("%1 failed: %2").arg(context, xlStatusToString(status));
    setError(msg);
    emit errorOccurred(msg);
    return CANResult::Failure(msg);
}

} // namespace CANManager
