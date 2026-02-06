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

    // Interface Type
    m_interfaceTypeCombo = new QComboBox;
    m_interfaceTypeCombo->addItems({"PEAK PCAN", "Vector", "SocketCAN", "Custom"});
    form->addRow(tr("Interface Type:"), m_interfaceTypeCombo);

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

    // Enable/disable FD bitrate based on FD checkbox
    connect(m_fdEnabledCheck, &QCheckBox::toggled, m_fdBitrateCombo, &QComboBox::setEnabled);
}

void CANConfigWidget::setConfig(const CANPortConfig& cfg)
{
    m_interfaceTypeCombo->setCurrentText(cfg.interfaceType);
    m_deviceEdit->setText(cfg.device);
    m_bitrateCombo->setCurrentText(QString::number(cfg.bitrate));
    m_fdEnabledCheck->setChecked(cfg.fdEnabled);
    m_fdBitrateCombo->setCurrentText(QString::number(cfg.fdDataBitrate));
    m_fdBitrateCombo->setEnabled(cfg.fdEnabled);
}

CANPortConfig CANConfigWidget::config() const
{
    CANPortConfig cfg;
    cfg.interfaceType = m_interfaceTypeCombo->currentText();
    cfg.device = m_deviceEdit->text();
    cfg.bitrate = m_bitrateCombo->currentText().toInt();
    cfg.fdEnabled = m_fdEnabledCheck->isChecked();
    cfg.fdDataBitrate = m_fdBitrateCombo->currentText().toInt();
    return cfg;
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

        layout->addStretch();

        innerTabs->addTab(page, tr("Port %1").arg(i + 1));
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
    layout->addStretch();

    parent->addTab(page, tr("Power Supply"));
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

    layout->addStretch();

    parent->addTab(page, tr("Modbus Relay"));
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
