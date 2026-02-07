#include "SerialConfigWidget.h"
#include "HWConfigManager.h"    // pulls in SerialManager::SerialPortConfig

#include <QFormLayout>
#include <QHBoxLayout>
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
