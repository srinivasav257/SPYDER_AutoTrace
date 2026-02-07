/**
 * @file tst_HWConfigManager.cpp
 * @brief Unit tests for HWConfigManager save/load round-trip.
 *
 * Uses a temporary QSettings scope so tests don't pollute the real registry.
 */

#include <gtest/gtest.h>
#include "HWConfigManager.h"
#include <QCoreApplication>
#include <QSettings>

// ============================================================================
// Fixture — ensure QCoreApplication exists and use temp QSettings scope
// ============================================================================

class HWConfigManagerTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        if (!QCoreApplication::instance()) {
            static int argc = 1;
            static char arg0[] = "test";
            static char* argv[] = {arg0, nullptr};
            static QCoreApplication app(argc, argv);
        }
        // Use a test-specific org/app so we don't pollute real settings
        QCoreApplication::setOrganizationName("SPYDER_UnitTest");
        QCoreApplication::setApplicationName("HWConfigTest");
    }

    void SetUp() override
    {
        // Clear any leftover settings before each test
        QSettings s;
        s.clear();
    }

    void TearDown() override
    {
        QSettings s;
        s.clear();
    }

    HWConfigManager& mgr() { return HWConfigManager::instance(); }
};

// ============================================================================
// Serial debug port round-trip
// ============================================================================

TEST_F(HWConfigManagerTest, SerialDebugPort_SetGet)
{
    SerialDebugPortConfig cfg;
    cfg.customName = "My Debug Port";
    cfg.serial.portName = "COM7";
    cfg.serial.baudRate = 115200;

    mgr().setSerialDebugPort(0, cfg);

    SerialDebugPortConfig result = mgr().serialDebugPort(0);
    EXPECT_EQ(result.customName, "My Debug Port");
    EXPECT_EQ(result.serial.portName, "COM7");
    EXPECT_EQ(result.serial.baudRate, 115200);
}

TEST_F(HWConfigManagerTest, SerialDebugPort_InvalidIndex)
{
    SerialDebugPortConfig result = mgr().serialDebugPort(-1);
    EXPECT_TRUE(result.customName.isEmpty());

    result = mgr().serialDebugPort(99);
    EXPECT_TRUE(result.customName.isEmpty());
}

TEST_F(HWConfigManagerTest, SerialDebugPort_SaveLoadRoundTrip)
{
    SerialDebugPortConfig cfg;
    cfg.customName = "DUT UART";
    cfg.serial.portName = "COM3";
    cfg.serial.baudRate = 9600;
    cfg.serial.parity = QSerialPort::EvenParity;
    cfg.serial.stopBits = QSerialPort::TwoStop;
    mgr().setSerialDebugPort(1, cfg);

    mgr().save();

    // Overwrite in memory
    SerialDebugPortConfig blank;
    blank.customName = "Overwritten";
    blank.serial.portName = "COM99";
    mgr().setSerialDebugPort(1, blank);

    // Load should restore
    mgr().load();

    SerialDebugPortConfig result = mgr().serialDebugPort(1);
    EXPECT_EQ(result.customName, "DUT UART");
    EXPECT_EQ(result.serial.portName, "COM3");
    EXPECT_EQ(result.serial.baudRate, 9600);
}

// ============================================================================
// CAN port round-trip
// ============================================================================

TEST_F(HWConfigManagerTest, CANPort_SetGet)
{
    CANPortConfig cfg;
    cfg.customName = "Vehicle Bus";
    cfg.interfaceType = "Vector";
    cfg.device = "PCAN_USBBUS2";
    cfg.channel = 2;
    cfg.bitrate = 250000;
    cfg.fdEnabled = true;
    cfg.fdDataBitrate = 4000000;
    cfg.dbcFilePath = "C:/dbc/vehicle.dbc";

    mgr().setCanPort(0, cfg);

    CANPortConfig result = mgr().canPort(0);
    EXPECT_EQ(result.customName, "Vehicle Bus");
    EXPECT_EQ(result.interfaceType, "Vector");
    EXPECT_EQ(result.device, "PCAN_USBBUS2");
    EXPECT_EQ(result.channel, 2);
    EXPECT_EQ(result.bitrate, 250000);
    EXPECT_TRUE(result.fdEnabled);
    EXPECT_EQ(result.fdDataBitrate, 4000000);
    EXPECT_EQ(result.dbcFilePath, "C:/dbc/vehicle.dbc");
}

TEST_F(HWConfigManagerTest, CANPort_InvalidIndex)
{
    CANPortConfig result = mgr().canPort(-1);
    EXPECT_TRUE(result.customName.isEmpty());
}

TEST_F(HWConfigManagerTest, CANPort_SaveLoadRoundTrip)
{
    CANPortConfig cfg;
    cfg.customName = "Test CAN";
    cfg.interfaceType = "PEAK";
    cfg.device = "PCAN_USBBUS1";
    cfg.bitrate = 500000;
    cfg.fdEnabled = false;
    cfg.vectorHwType = 57;
    cfg.vectorHwIndex = 1;
    cfg.vectorHwChannel = 0;
    cfg.vectorChannelIdx = 3;
    cfg.vectorChannelMask = 8;
    cfg.dbcFilePath = "/tmp/test.dbc";
    mgr().setCanPort(0, cfg);

    mgr().save();

    // Overwrite
    CANPortConfig blank;
    blank.customName = "Blank";
    mgr().setCanPort(0, blank);

    mgr().load();

    CANPortConfig result = mgr().canPort(0);
    EXPECT_EQ(result.customName, "Test CAN");
    EXPECT_EQ(result.interfaceType, "PEAK");
    EXPECT_EQ(result.bitrate, 500000);
    EXPECT_EQ(result.vectorHwType, 57);
    EXPECT_EQ(result.vectorChannelIdx, 3);
    EXPECT_EQ(result.dbcFilePath, "/tmp/test.dbc");
}

// ============================================================================
// Power supply round-trip
// ============================================================================

TEST_F(HWConfigManagerTest, PowerSupply_SetGet)
{
    PowerSupplyConfig cfg;
    cfg.customName = "Lab PSU";
    cfg.serial.portName = "COM10";
    cfg.serial.baudRate = 9600;

    mgr().setPowerSupply(cfg);

    PowerSupplyConfig result = mgr().powerSupply();
    EXPECT_EQ(result.customName, "Lab PSU");
    EXPECT_EQ(result.serial.portName, "COM10");
}

// ============================================================================
// Modbus relay round-trip
// ============================================================================

TEST_F(HWConfigManagerTest, ModbusRelay_SetGet)
{
    ModbusRelayConfig cfg;
    cfg.customName = "Relay Board";
    cfg.serial.portName = "COM5";
    cfg.serial.baudRate = 19200;
    cfg.modbusAddress = 42;

    mgr().setModbusRelay(cfg);

    ModbusRelayConfig result = mgr().modbusRelay();
    EXPECT_EQ(result.customName, "Relay Board");
    EXPECT_EQ(result.serial.portName, "COM5");
    EXPECT_EQ(result.serial.baudRate, 19200);
    EXPECT_EQ(result.modbusAddress, 42);
}

// ============================================================================
// Alias resolution
// ============================================================================

TEST_F(HWConfigManagerTest, PortAliasResolution)
{
    SerialDebugPortConfig cfg;
    cfg.customName = "DUT Debug";
    cfg.serial.portName = "COM8";
    mgr().setSerialDebugPort(0, cfg);

    // Resolve by custom name
    EXPECT_EQ(mgr().resolvePortAlias("DUT Debug"), "COM8");

    // Resolve by "Name (COMx)" format
    EXPECT_EQ(mgr().resolvePortAlias("DUT Debug (COM8)"), "COM8");

    // Unknown alias returns as-is
    EXPECT_EQ(mgr().resolvePortAlias("Unknown"), "Unknown");
}

TEST_F(HWConfigManagerTest, CANAliasResolution)
{
    CANPortConfig cfg;
    cfg.customName = "Vehicle CAN";
    cfg.device = "PCAN_USBBUS1";
    mgr().setCanPort(0, cfg);

    EXPECT_EQ(mgr().resolveCANAlias("Vehicle CAN"), "PCAN_USBBUS1");
    EXPECT_EQ(mgr().resolveCANAlias("Unknown"), "Unknown");
}

TEST_F(HWConfigManagerTest, AllPortAliases)
{
    SerialDebugPortConfig cfg;
    cfg.customName = "Port A";
    cfg.serial.portName = "COM1";
    mgr().setSerialDebugPort(0, cfg);

    QStringList aliases = mgr().allPortAliases();
    bool found = false;
    for (const auto& a : aliases) {
        if (a.contains("Port A")) found = true;
    }
    EXPECT_TRUE(found);
}

TEST_F(HWConfigManagerTest, AllCANAliases)
{
    CANPortConfig cfg;
    cfg.customName = "Bus A";
    mgr().setCanPort(0, cfg);

    QStringList aliases = mgr().allCANAliases();
    EXPECT_TRUE(aliases.contains("Bus A"));
}
