#include "HWConfigDialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSerialPortInfo>

// ===========================================================================
// SerialConfigWidget
// ===========================================================================

SerialConfigWidget::SerialConfigWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* form = new QFormLayout(this);

    // COM Port
    auto* portRow = new QHBoxLayout;
    m_portCombo = new QComboBox;
    m_portCombo->setEditable(true);
    m_refreshBtn = new QPushButton(tr("Refresh"));
    portRow->addWidget(m_portCombo, 1);
    portRow->addWidget(m_refreshBtn);
    form->addRow(tr("COM Port:"), portRow);

    // Baud Rate
    m_baudCombo = new QComboBox;
    m_baudCombo->addItems({"9600", "19200", "38400", "57600", "115200",
                           "230400", "460800", "921600"});
    m_baudCombo->setCurrentText("115200");
    form->addRow(tr("Baud Rate:"), m_baudCombo);

    // Data Bits
    m_dataBitsCombo = new QComboBox;
    m_dataBitsCombo->addItems({"5", "6", "7", "8"});
    m_dataBitsCombo->setCurrentText("8");
    form->addRow(tr("Data Bits:"), m_dataBitsCombo);

    // Parity
    m_parityCombo = new QComboBox;
    m_parityCombo->addItems({"None", "Even", "Odd", "Space", "Mark"});
    form->addRow(tr("Parity:"), m_parityCombo);

    // Stop Bits
    m_stopBitsCombo = new QComboBox;
    m_stopBitsCombo->addItems({"1", "1.5", "2"});
    form->addRow(tr("Stop Bits:"), m_stopBitsCombo);

    // Flow Control
    m_flowControlCombo = new QComboBox;
    m_flowControlCombo->addItems({"None", "Hardware", "Software"});
    form->addRow(tr("Flow Control:"), m_flowControlCombo);

    // Populate ports
    refreshPorts();

    connect(m_refreshBtn, &QPushButton::clicked, this, &SerialConfigWidget::refreshPorts);
}

void SerialConfigWidget::refreshPorts()
{
    QString current = m_portCombo->currentText();
    m_portCombo->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& info : ports) {
        m_portCombo->addItem(
            QString("%1 - %2").arg(info.portName(), info.description()),
            info.portName());
    }
    // Restore previous selection or set it as editable text
    int idx = m_portCombo->findData(current);
    if (idx >= 0) {
        m_portCombo->setCurrentIndex(idx);
    } else if (!current.isEmpty()) {
        m_portCombo->setEditText(current);
    }
}

void SerialConfigWidget::setConfig(const SerialManager::SerialPortConfig& cfg)
{
    // Set port - try data match first, then set as text
    int idx = m_portCombo->findData(cfg.portName);
    if (idx >= 0)
        m_portCombo->setCurrentIndex(idx);
    else
        m_portCombo->setEditText(cfg.portName);

    m_baudCombo->setCurrentText(QString::number(cfg.baudRate));
    m_dataBitsCombo->setCurrentText(QString::number(static_cast<int>(cfg.dataBits)));

    // Parity
    switch (cfg.parity) {
    case QSerialPort::EvenParity:  m_parityCombo->setCurrentText("Even"); break;
    case QSerialPort::OddParity:   m_parityCombo->setCurrentText("Odd"); break;
    case QSerialPort::SpaceParity: m_parityCombo->setCurrentText("Space"); break;
    case QSerialPort::MarkParity:  m_parityCombo->setCurrentText("Mark"); break;
    default:                       m_parityCombo->setCurrentText("None"); break;
    }

    // Stop Bits
    switch (cfg.stopBits) {
    case QSerialPort::OneAndHalfStop: m_stopBitsCombo->setCurrentText("1.5"); break;
    case QSerialPort::TwoStop:       m_stopBitsCombo->setCurrentText("2"); break;
    default:                         m_stopBitsCombo->setCurrentText("1"); break;
    }

    // Flow Control
    switch (cfg.flowControl) {
    case QSerialPort::HardwareControl: m_flowControlCombo->setCurrentText("Hardware"); break;
    case QSerialPort::SoftwareControl: m_flowControlCombo->setCurrentText("Software"); break;
    default:                           m_flowControlCombo->setCurrentText("None"); break;
    }
}

SerialManager::SerialPortConfig SerialConfigWidget::config() const
{
    SerialManager::SerialPortConfig cfg;

    // Get port name from combo data if available, otherwise text
    QVariant portData = m_portCombo->currentData();
    cfg.portName = portData.isValid() ? portData.toString() : m_portCombo->currentText();

    cfg.baudRate = m_baudCombo->currentText().toInt();
    cfg.dataBits = SerialManager::SerialPortConfig::dataBitsFromInt(
        m_dataBitsCombo->currentText().toInt());
    cfg.parity = SerialManager::SerialPortConfig::parityFromString(
        m_parityCombo->currentText());
    QString stopBitsStr = m_stopBitsCombo->currentText();
    if (stopBitsStr == "1.5")
        cfg.stopBits = QSerialPort::OneAndHalfStop;
    else if (stopBitsStr == "2")
        cfg.stopBits = QSerialPort::TwoStop;
    else
        cfg.stopBits = QSerialPort::OneStop;
    cfg.flowControl = SerialManager::SerialPortConfig::flowControlFromString(
        m_flowControlCombo->currentText());
    return cfg;
}

// ===========================================================================
// CANConfigWidget
// ===========================================================================

CANConfigWidget::CANConfigWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* form = new QFormLayout(this);

    // Alias (Device Name for tests)
    m_aliasEdit = new QLineEdit;
    m_aliasEdit->setPlaceholderText(tr("e.g., Vehicle_CAN_HS"));
    form->addRow(tr("Alias (Device Name):"), m_aliasEdit);

    // Interface Type
    m_interfaceTypeCombo = new QComboBox;
    m_interfaceTypeCombo->addItems({"PEAK PCAN", "Vector", "SocketCAN", "Custom"});
    form->addRow(tr("Interface Type:"), m_interfaceTypeCombo);

    // Channel
    m_channelSpin = new QSpinBox;
    m_channelSpin->setRange(1, 16);
    m_channelSpin->setValue(1);
    form->addRow(tr("Channel:"), m_channelSpin);

    // Device
    m_deviceEdit = new QLineEdit("PCAN_USBBUS1");
    m_deviceEdit->setPlaceholderText(tr("e.g., PCAN_USBBUS1, can0"));
    form->addRow(tr("Device:"), m_deviceEdit);

    // Bitrate
    m_bitrateCombo = new QComboBox;
    m_bitrateCombo->addItems({"125000", "250000", "500000", "1000000"});
    m_bitrateCombo->setCurrentText("500000");
    form->addRow(tr("Bitrate (bps):"), m_bitrateCombo);

    // CAN FD
    m_fdEnabledCheck = new QCheckBox(tr("Enable CAN FD"));
    form->addRow(tr("CAN FD:"), m_fdEnabledCheck);

    // FD Data Bitrate
    m_fdBitrateCombo = new QComboBox;
    m_fdBitrateCombo->addItems({"1000000", "2000000", "4000000", "5000000", "8000000"});
    m_fdBitrateCombo->setCurrentText("2000000");
    m_fdBitrateCombo->setEnabled(false);
    form->addRow(tr("FD Data Bitrate (bps):"), m_fdBitrateCombo);

    // Connect/Disconnect
    auto* connLayout = new QHBoxLayout;
    m_connectBtn = new QPushButton(tr("Connect"));
    m_disconnectBtn = new QPushButton(tr("Disconnect"));
    m_disconnectBtn->setEnabled(false);
    m_statusLabel = new QLabel(tr("Disconnected"));
    m_statusLabel->setStyleSheet("color: gray;");
    connLayout->addWidget(m_connectBtn);
    connLayout->addWidget(m_disconnectBtn);
    connLayout->addWidget(m_statusLabel);
    connLayout->addStretch();
    form->addRow(tr("Connection:"), connLayout);

    // Enable/disable FD bitrate based on FD checkbox
    connect(m_fdEnabledCheck, &QCheckBox::toggled, m_fdBitrateCombo, &QComboBox::setEnabled);

    // Connect/Disconnect signals
    connect(m_connectBtn, &QPushButton::clicked, this, &CANConfigWidget::connectRequested);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &CANConfigWidget::disconnectRequested);
}

void CANConfigWidget::setConfig(const CANPortConfig& cfg)
{
    m_aliasEdit->setText(cfg.customName);
    m_interfaceTypeCombo->setCurrentText(cfg.interfaceType);
    m_deviceEdit->setText(cfg.device);
    m_channelSpin->setValue(cfg.channel);
    m_bitrateCombo->setCurrentText(QString::number(cfg.bitrate));
    m_fdEnabledCheck->setChecked(cfg.fdEnabled);
    m_fdBitrateCombo->setCurrentText(QString::number(cfg.fdDataBitrate));
    m_fdBitrateCombo->setEnabled(cfg.fdEnabled);
}

CANPortConfig CANConfigWidget::config() const
{
    CANPortConfig cfg;
    cfg.customName = m_aliasEdit->text();
    cfg.interfaceType = m_interfaceTypeCombo->currentText();
    cfg.device = m_deviceEdit->text();
    cfg.channel = m_channelSpin->value();
    cfg.bitrate = m_bitrateCombo->currentText().toInt();
    cfg.fdEnabled = m_fdEnabledCheck->isChecked();
    cfg.fdDataBitrate = m_fdBitrateCombo->currentText().toInt();
    return cfg;
}

void CANConfigWidget::setConnectionStatus(bool connected, const QString& message)
{
    m_connectBtn->setEnabled(!connected);
    m_disconnectBtn->setEnabled(connected);
    if (connected) {
        m_statusLabel->setText(tr("Connected"));
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        m_statusLabel->setText(message.isEmpty() ? tr("Disconnected") : message);
        m_statusLabel->setStyleSheet(message.isEmpty() ? "color: gray;" : "color: red;");
    }
}

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
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);
        layout->addWidget(m_canTabs[i]);
        layout->addStretch();
        innerTabs->addTab(page, tr("CAN %1 (HS/FD)").arg(i + 1));

        // Connect handler (CAN bus connection - stub for now)
        connect(m_canTabs[i], &CANConfigWidget::connectRequested, this, [this, i]() {
            // TODO: Implement actual CAN bus connection via CAN driver
            m_canTabs[i]->setConnectionStatus(true);
        });

        // Disconnect handler
        connect(m_canTabs[i], &CANConfigWidget::disconnectRequested, this, [this, i]() {
            // TODO: Implement actual CAN bus disconnection
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
    emit mgr.configChanged();
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
