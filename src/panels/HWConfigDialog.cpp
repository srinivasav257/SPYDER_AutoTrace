#include "HWConfigDialog.h"

#include <CANManager.h>
#include <VectorCANDriver.h>
#include <DBCManager.h>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>

// ===========================================================================
// HWConfigDialog
// ===========================================================================

HWConfigDialog::HWConfigDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Hardware Configuration"));
    setMinimumSize(500, 450);
    resize(560, 520);

    auto* mainLayout = new QVBoxLayout(this);

    // Top-level tab widget
    auto* topTabs = new QTabWidget;
    mainLayout->addWidget(topTabs);

    setupSerialDebugTab(topTabs);
    setupCANTab(topTabs);
    setupPowerSupplyTab(topTabs);
    setupModbusTab(topTabs);

    // Dialog buttons
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    mainLayout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &HWConfigDialog::onOk);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &HWConfigDialog::onApply);

    loadFromManager();
}

void HWConfigDialog::setupSerialDebugTab(QTabWidget* parent)
{
    auto* innerTabs = new QTabWidget;
    parent->addTab(innerTabs, tr("Serial Debug Ports"));

    for (int i = 0; i < HWConfigManager::SERIAL_PORT_COUNT; ++i) {
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);

        // Custom name field
        auto* nameLayout = new QFormLayout;
        m_serialDebugTabs[i].customName = new QLineEdit;
        m_serialDebugTabs[i].customName->setPlaceholderText(
            tr("e.g., DUT Debug, ECU Console"));
        nameLayout->addRow(tr("Port Name (Alias):"), m_serialDebugTabs[i].customName);
        layout->addLayout(nameLayout);

        // Serial config
        m_serialDebugTabs[i].serialConfig = new SerialConfigWidget;
        layout->addWidget(m_serialDebugTabs[i].serialConfig);

        // Connect/Disconnect buttons
        auto* connLayout = new QHBoxLayout;
        m_serialDebugTabs[i].connectBtn = new QPushButton(tr("Connect"));
        m_serialDebugTabs[i].disconnectBtn = new QPushButton(tr("Disconnect"));
        m_serialDebugTabs[i].disconnectBtn->setEnabled(false);
        m_serialDebugTabs[i].statusLabel = new QLabel(tr("Disconnected"));
        m_serialDebugTabs[i].statusLabel->setStyleSheet("color: gray;");
        connLayout->addWidget(m_serialDebugTabs[i].connectBtn);
        connLayout->addWidget(m_serialDebugTabs[i].disconnectBtn);
        connLayout->addWidget(m_serialDebugTabs[i].statusLabel);
        connLayout->addStretch();
        layout->addLayout(connLayout);

        layout->addStretch();

        innerTabs->addTab(page, tr("Port %1").arg(i + 1));

        // Connect handler
        connect(m_serialDebugTabs[i].connectBtn, &QPushButton::clicked, this, [this, i]() {
            auto cfg = m_serialDebugTabs[i].serialConfig->config();
            if (cfg.portName.isEmpty()) return;
            auto& serial = SerialManager::SerialPortManager::instance();
            serial.setPortConfig(cfg.portName, cfg);
            auto result = serial.openPort(cfg.portName);
            bool connected = result.success;
            m_serialDebugTabs[i].connectBtn->setEnabled(!connected);
            m_serialDebugTabs[i].disconnectBtn->setEnabled(connected);
            m_serialDebugTabs[i].statusLabel->setText(
                connected ? tr("Connected") : tr("Failed: %1").arg(result.errorMessage));
            m_serialDebugTabs[i].statusLabel->setStyleSheet(
                connected ? "color: green; font-weight: bold;" : "color: red;");
        });

        // Disconnect handler
        connect(m_serialDebugTabs[i].disconnectBtn, &QPushButton::clicked, this, [this, i]() {
            auto cfg = m_serialDebugTabs[i].serialConfig->config();
            SerialManager::SerialPortManager::instance().closePort(cfg.portName);
            m_serialDebugTabs[i].connectBtn->setEnabled(true);
            m_serialDebugTabs[i].disconnectBtn->setEnabled(false);
            m_serialDebugTabs[i].statusLabel->setText(tr("Disconnected"));
            m_serialDebugTabs[i].statusLabel->setStyleSheet("color: gray;");
        });
    }
}

void HWConfigDialog::setupCANTab(QTabWidget* parent)
{
    auto* innerTabs = new QTabWidget;
    parent->addTab(innerTabs, tr("CAN Ports"));

    for (int i = 0; i < HWConfigManager::CAN_PORT_COUNT; ++i) {
        m_canTabs[i] = new CANConfigWidget;
        m_canTabs[i]->setChannelIndex(i);  // Associate with CAN channel for DBC
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);
        layout->addWidget(m_canTabs[i]);
        layout->addStretch();
        innerTabs->addTab(page, tr("CAN %1 (HS/FD)").arg(i + 1));

        // Connect handler (CAN bus connection via CANManager)
        connect(m_canTabs[i], &CANConfigWidget::connectRequested, this, [this, i]() {
            auto cfg = m_canTabs[i]->config();
            QString slotName = QString("CAN %1").arg(i + 1);

            auto& canMgr = CANManager::CANBusManager::instance();

            if (cfg.interfaceType == "Vector") {
                if (cfg.vectorChannelIdx < 0 || cfg.vectorChannelMask == 0) {
                    m_canTabs[i]->setConnectionStatus(
                        false,
                        tr("No Vector channel selected. Click \"Detect HW\" first."));
                    return;
                }

                auto* vectorDrv = canMgr.vectorDriver();
                if (!vectorDrv || !vectorDrv->initialize()) {
                    m_canTabs[i]->setConnectionStatus(false, tr("Vector driver not available"));
                    return;
                }

                // Build channel info from stored config
                CANManager::CANChannelInfo chInfo;
                chInfo.hwType       = cfg.vectorHwType;
                chInfo.hwIndex      = cfg.vectorHwIndex;
                chInfo.hwChannel    = cfg.vectorHwChannel;
                chInfo.channelIndex = cfg.vectorChannelIdx;
                chInfo.channelMask  = cfg.vectorChannelMask;
                chInfo.name         = cfg.device;

                CANManager::CANBusConfig busConfig;
                busConfig.bitrate       = cfg.bitrate;
                busConfig.fdEnabled     = cfg.fdEnabled;
                busConfig.fdDataBitrate = cfg.fdDataBitrate;

                auto result = canMgr.openSlot(slotName, vectorDrv, chInfo, busConfig);
                if (result.success) {
                    m_canTabs[i]->setConnectionStatus(true);
                } else {
                    m_canTabs[i]->setConnectionStatus(false, result.errorMessage);
                }
            } else {
                // Non-Vector: placeholder for PEAK/SocketCAN/Custom
                m_canTabs[i]->setConnectionStatus(false, tr("Driver not implemented yet"));
            }
        });

        // Disconnect handler
        connect(m_canTabs[i], &CANConfigWidget::disconnectRequested, this, [this, i]() {
            QString slotName = QString("CAN %1").arg(i + 1);
            CANManager::CANBusManager::instance().closeSlot(slotName);
            m_canTabs[i]->setConnectionStatus(false);
        });
    }
}

void HWConfigDialog::setupPowerSupplyTab(QTabWidget* parent)
{
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);

    auto* infoLabel = new QLabel(tr("<b>TENMA 72-2540 Programmable Power Supply</b><br>"
                                    "Configure the serial port used to communicate with "
                                    "the power supply."));
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    // Custom name
    auto* nameLayout = new QFormLayout;
    m_powerSupplyName = new QLineEdit;
    m_powerSupplyName->setPlaceholderText(tr("e.g., Bench PSU, DUT Power"));
    nameLayout->addRow(tr("Custom Name:"), m_powerSupplyName);
    layout->addLayout(nameLayout);

    m_powerSupplySerial = new SerialConfigWidget;
    layout->addWidget(m_powerSupplySerial);

    // Connect/Disconnect buttons
    auto* connLayout = new QHBoxLayout;
    m_powerSupplyConnectBtn = new QPushButton(tr("Connect"));
    m_powerSupplyDisconnectBtn = new QPushButton(tr("Disconnect"));
    m_powerSupplyDisconnectBtn->setEnabled(false);
    m_powerSupplyStatusLabel = new QLabel(tr("Disconnected"));
    m_powerSupplyStatusLabel->setStyleSheet("color: gray;");
    connLayout->addWidget(m_powerSupplyConnectBtn);
    connLayout->addWidget(m_powerSupplyDisconnectBtn);
    connLayout->addWidget(m_powerSupplyStatusLabel);
    connLayout->addStretch();
    layout->addLayout(connLayout);

    layout->addStretch();

    parent->addTab(page, tr("Power Supply"));

    // Connect handler
    connect(m_powerSupplyConnectBtn, &QPushButton::clicked, this, [this]() {
        auto cfg = m_powerSupplySerial->config();
        if (cfg.portName.isEmpty()) return;
        auto& serial = SerialManager::SerialPortManager::instance();
        serial.setPortConfig(cfg.portName, cfg);
        auto result = serial.openPort(cfg.portName);
        bool connected = result.success;
        m_powerSupplyConnectBtn->setEnabled(!connected);
        m_powerSupplyDisconnectBtn->setEnabled(connected);
        m_powerSupplyStatusLabel->setText(
            connected ? tr("Connected") : tr("Failed: %1").arg(result.errorMessage));
        m_powerSupplyStatusLabel->setStyleSheet(
            connected ? "color: green; font-weight: bold;" : "color: red;");
    });

    // Disconnect handler
    connect(m_powerSupplyDisconnectBtn, &QPushButton::clicked, this, [this]() {
        auto cfg = m_powerSupplySerial->config();
        SerialManager::SerialPortManager::instance().closePort(cfg.portName);
        m_powerSupplyConnectBtn->setEnabled(true);
        m_powerSupplyDisconnectBtn->setEnabled(false);
        m_powerSupplyStatusLabel->setText(tr("Disconnected"));
        m_powerSupplyStatusLabel->setStyleSheet("color: gray;");
    });
}

void HWConfigDialog::setupModbusTab(QTabWidget* parent)
{
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);

    auto* infoLabel = new QLabel(tr("<b>Modbus Serial Relay</b><br>"
                                    "Configure the serial port and Modbus address for "
                                    "the relay module."));
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    // Custom name
    auto* nameLayout = new QFormLayout;
    m_modbusName = new QLineEdit;
    m_modbusName->setPlaceholderText(tr("e.g., Relay Board, IO Control"));
    nameLayout->addRow(tr("Custom Name:"), m_modbusName);
    layout->addLayout(nameLayout);

    m_modbusSerial = new SerialConfigWidget;
    layout->addWidget(m_modbusSerial);

    // Modbus address
    auto* addrLayout = new QFormLayout;
    m_modbusAddress = new QSpinBox;
    m_modbusAddress->setRange(1, 247);
    m_modbusAddress->setValue(1);
    addrLayout->addRow(tr("Modbus Address:"), m_modbusAddress);
    layout->addLayout(addrLayout);

    // Connect/Disconnect buttons
    auto* connLayout = new QHBoxLayout;
    m_modbusConnectBtn = new QPushButton(tr("Connect"));
    m_modbusDisconnectBtn = new QPushButton(tr("Disconnect"));
    m_modbusDisconnectBtn->setEnabled(false);
    m_modbusStatusLabel = new QLabel(tr("Disconnected"));
    m_modbusStatusLabel->setStyleSheet("color: gray;");
    connLayout->addWidget(m_modbusConnectBtn);
    connLayout->addWidget(m_modbusDisconnectBtn);
    connLayout->addWidget(m_modbusStatusLabel);
    connLayout->addStretch();
    layout->addLayout(connLayout);

    layout->addStretch();

    parent->addTab(page, tr("Modbus Relay"));

    // Connect handler
    connect(m_modbusConnectBtn, &QPushButton::clicked, this, [this]() {
        auto cfg = m_modbusSerial->config();
        if (cfg.portName.isEmpty()) return;
        auto& serial = SerialManager::SerialPortManager::instance();
        serial.setPortConfig(cfg.portName, cfg);
        auto result = serial.openPort(cfg.portName);
        bool connected = result.success;
        m_modbusConnectBtn->setEnabled(!connected);
        m_modbusDisconnectBtn->setEnabled(connected);
        m_modbusStatusLabel->setText(
            connected ? tr("Connected") : tr("Failed: %1").arg(result.errorMessage));
        m_modbusStatusLabel->setStyleSheet(
            connected ? "color: green; font-weight: bold;" : "color: red;");
    });

    // Disconnect handler
    connect(m_modbusDisconnectBtn, &QPushButton::clicked, this, [this]() {
        auto cfg = m_modbusSerial->config();
        SerialManager::SerialPortManager::instance().closePort(cfg.portName);
        m_modbusConnectBtn->setEnabled(true);
        m_modbusDisconnectBtn->setEnabled(false);
        m_modbusStatusLabel->setText(tr("Disconnected"));
        m_modbusStatusLabel->setStyleSheet("color: gray;");
    });
}

void HWConfigDialog::loadFromManager()
{
    auto& mgr = HWConfigManager::instance();

    // Serial Debug Ports
    for (int i = 0; i < HWConfigManager::SERIAL_PORT_COUNT; ++i) {
        auto cfg = mgr.serialDebugPort(i);
        m_serialDebugTabs[i].customName->setText(cfg.customName);
        m_serialDebugTabs[i].serialConfig->setConfig(cfg.serial);
    }

    // CAN Ports
    for (int i = 0; i < HWConfigManager::CAN_PORT_COUNT; ++i) {
        m_canTabs[i]->setConfig(mgr.canPort(i));
    }

    // Power Supply
    auto ps = mgr.powerSupply();
    m_powerSupplyName->setText(ps.customName);
    m_powerSupplySerial->setConfig(ps.serial);

    // Modbus Relay
    auto modbus = mgr.modbusRelay();
    m_modbusName->setText(modbus.customName);
    m_modbusSerial->setConfig(modbus.serial);
    m_modbusAddress->setValue(modbus.modbusAddress);
}

void HWConfigDialog::saveToManager()
{
    auto& mgr = HWConfigManager::instance();

    // Serial Debug Ports
    for (int i = 0; i < HWConfigManager::SERIAL_PORT_COUNT; ++i) {
        SerialDebugPortConfig cfg;
        cfg.customName = m_serialDebugTabs[i].customName->text();
        cfg.serial = m_serialDebugTabs[i].serialConfig->config();
        mgr.setSerialDebugPort(i, cfg);
    }

    // CAN Ports
    for (int i = 0; i < HWConfigManager::CAN_PORT_COUNT; ++i) {
        mgr.setCanPort(i, m_canTabs[i]->config());
    }

    // Power Supply
    PowerSupplyConfig psCfg;
    psCfg.customName = m_powerSupplyName->text();
    psCfg.serial = m_powerSupplySerial->config();
    mgr.setPowerSupply(psCfg);

    // Modbus Relay
    ModbusRelayConfig mbCfg;
    mbCfg.customName = m_modbusName->text();
    mbCfg.serial = m_modbusSerial->config();
    mbCfg.modbusAddress = m_modbusAddress->value();
    mgr.setModbusRelay(mbCfg);

    mgr.save();
    mgr.applyToSerialManager();
}

void HWConfigDialog::onApply()
{
    saveToManager();
}

void HWConfigDialog::onOk()
{
    saveToManager();
    accept();
}
