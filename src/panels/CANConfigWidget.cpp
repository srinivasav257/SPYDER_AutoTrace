#include "CANConfigWidget.h"

#include <CANManager.h>
#include <VectorCANDriver.h>
#include <DBCManager.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QRegularExpression>

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
    m_interfaceTypeCombo->addItems({"Vector", "PEAK PCAN", "SocketCAN", "Custom"});
    form->addRow(tr("Interface Type:"), m_interfaceTypeCombo);

    // ----- Vector Channel Mapping (shown when Vector is selected) -----
    m_channelMappingRow = new QWidget;
    auto* mappingLayout = new QHBoxLayout(m_channelMappingRow);
    mappingLayout->setContentsMargins(0, 0, 0, 0);
    m_channelMappingCombo = new QComboBox;
    m_channelMappingCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_channelMappingCombo->setMinimumWidth(250);
    m_detectHWBtn = new QPushButton(tr("Detect HW"));
    m_detectHWBtn->setToolTip(tr("Scan for connected Vector CAN hardware"));
    mappingLayout->addWidget(m_channelMappingCombo, 1);
    mappingLayout->addWidget(m_detectHWBtn);
    form->addRow(tr("Channel Mapping:"), m_channelMappingRow);

    // ----- Manual Device / Channel (shown when non-Vector) -----
    m_deviceRow = new QWidget;
    auto* deviceLayout = new QHBoxLayout(m_deviceRow);
    deviceLayout->setContentsMargins(0, 0, 0, 0);
    m_deviceEdit = new QLineEdit("PCAN_USBBUS1");
    m_deviceEdit->setPlaceholderText(tr("e.g., PCAN_USBBUS1, can0"));
    deviceLayout->addWidget(m_deviceEdit);
    form->addRow(tr("Device:"), m_deviceRow);

    m_channelRow = new QWidget;
    auto* chLayout = new QHBoxLayout(m_channelRow);
    chLayout->setContentsMargins(0, 0, 0, 0);
    m_channelSpin = new QSpinBox;
    m_channelSpin->setRange(1, 16);
    m_channelSpin->setValue(1);
    chLayout->addWidget(m_channelSpin);
    form->addRow(tr("Channel:"), m_channelRow);

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

    // ----- DBC File Association -----
    auto* dbcRow = new QHBoxLayout;
    m_dbcPathEdit = new QLineEdit;
    m_dbcPathEdit->setReadOnly(true);
    m_dbcPathEdit->setPlaceholderText(tr("No DBC file loaded"));
    m_dbcLoadBtn = new QPushButton(tr("Load DBC..."));
    m_dbcLoadBtn->setToolTip(tr("Browse for a DBC file to associate with this CAN channel"));
    m_dbcClearBtn = new QPushButton(tr("Clear"));
    m_dbcClearBtn->setToolTip(tr("Remove the DBC file association"));
    m_dbcClearBtn->setEnabled(false);
    dbcRow->addWidget(m_dbcPathEdit, 1);
    dbcRow->addWidget(m_dbcLoadBtn);
    dbcRow->addWidget(m_dbcClearBtn);
    form->addRow(tr("DBC File:"), dbcRow);

    m_dbcStatusLabel = new QLabel(tr("No DBC loaded"));
    m_dbcStatusLabel->setStyleSheet("color: gray; font-style: italic;");
    form->addRow(tr("DBC Status:"), m_dbcStatusLabel);

    // Enable/disable FD bitrate based on FD checkbox
    connect(m_fdEnabledCheck, &QCheckBox::toggled, m_fdBitrateCombo, &QComboBox::setEnabled);

    // Connect/Disconnect signals
    connect(m_connectBtn, &QPushButton::clicked, this, &CANConfigWidget::connectRequested);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &CANConfigWidget::disconnectRequested);

    // DBC load/clear
    connect(m_dbcLoadBtn, &QPushButton::clicked, this, &CANConfigWidget::onLoadDBCClicked);
    connect(m_dbcClearBtn, &QPushButton::clicked, this, &CANConfigWidget::onClearDBCClicked);

    // Listen for DBC load completion
    connect(&DBCManager::DBCDatabaseManager::instance(), &DBCManager::DBCDatabaseManager::loadFinished,
            this, &CANConfigWidget::onDBCLoadFinished);
    connect(&DBCManager::DBCDatabaseManager::instance(), &DBCManager::DBCDatabaseManager::loadProgress,
            this, [this](int channelIndex, const QString& status) {
        if (channelIndex == m_channelIndex)
            m_dbcStatusLabel->setText(status);
    });

    // Interface type switching: show/hide channel mapping vs manual fields
    connect(m_interfaceTypeCombo, &QComboBox::currentTextChanged,
            this, &CANConfigWidget::onInterfaceTypeChanged);

    // Detect HW button
    connect(m_detectHWBtn, &QPushButton::clicked, this, &CANConfigWidget::refreshVectorChannels);

    // Set initial visibility based on default interface type
    onInterfaceTypeChanged(m_interfaceTypeCombo->currentText());

    // Auto-detect Vector channels on creation
    QMetaObject::invokeMethod(this, &CANConfigWidget::refreshVectorChannels, Qt::QueuedConnection);
}

void CANConfigWidget::onInterfaceTypeChanged(const QString& type)
{
    bool isVector = (type == "Vector");
    m_channelMappingRow->setVisible(isVector);
    m_deviceRow->setVisible(!isVector);
    m_channelRow->setVisible(!isVector);
}

void CANConfigWidget::refreshVectorChannels()
{
    m_channelMappingCombo->clear();
    m_detectedChannels.clear();

    auto& canMgr = CANManager::CANBusManager::instance();
    auto* vectorDrv = canMgr.vectorDriver();
    if (!vectorDrv)
        return;

    // Initialize driver if needed
    if (!vectorDrv->initialize()) {
        m_channelMappingCombo->addItem(tr("(Vector driver not available)"));
        return;
    }

    auto channels = vectorDrv->detectChannels();
    if (channels.isEmpty()) {
        m_channelMappingCombo->addItem(tr("(No Vector CAN channels detected)"));
        return;
    }

    m_detectedChannels = channels;
    for (int i = 0; i < channels.size(); ++i) {
        const auto& ch = channels[i];
        QString label = QString("%1 — %2 Ch%3%4")
            .arg(ch.name)
            .arg(ch.hwTypeName)
            .arg(ch.hwChannel + 1)
            .arg(ch.supportsFD ? " [CAN FD]" : "");
        if (ch.serialNumber > 0)
            label += QString("  S/N:%1").arg(ch.serialNumber);

        m_channelMappingCombo->addItem(label, i);  // userData = index into m_detectedChannels
    }
}

void CANConfigWidget::setChannelIndex(int index)
{
    m_channelIndex = index;

    // Update DBC status from current DBCManager state
    auto& dbcMgr = DBCManager::DBCDatabaseManager::instance();
    if (dbcMgr.isLoaded(index)) {
        auto db = dbcMgr.database(index);
        m_dbcPathEdit->setText(dbcMgr.dbcFilePath(index));
        m_dbcClearBtn->setEnabled(true);
        m_dbcStatusLabel->setText(
            QString("Loaded: %1 messages, %2 signals")
                .arg(db ? db->messages.size() : 0)
                .arg(db ? db->totalSignalCount() : 0));
        m_dbcStatusLabel->setStyleSheet("color: green;");
    } else if (dbcMgr.isLoading(index)) {
        m_dbcPathEdit->setText(dbcMgr.dbcFilePath(index));
        m_dbcStatusLabel->setText(tr("Loading..."));
        m_dbcStatusLabel->setStyleSheet("color: orange;");
    }
}

void CANConfigWidget::onLoadDBCClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select DBC File"), QString(),
        tr("DBC Files (*.dbc);;All Files (*.*)"));

    if (filePath.isEmpty())
        return;

    m_dbcPathEdit->setText(filePath);
    m_dbcStatusLabel->setText(tr("Loading..."));
    m_dbcStatusLabel->setStyleSheet("color: orange;");
    m_dbcLoadBtn->setEnabled(false);
    m_dbcClearBtn->setEnabled(false);

    // Trigger background loading
    DBCManager::DBCDatabaseManager::instance().loadDBCFile(m_channelIndex, filePath);
}

void CANConfigWidget::onClearDBCClicked()
{
    DBCManager::DBCDatabaseManager::instance().unloadDBC(m_channelIndex);
    m_dbcPathEdit->clear();
    m_dbcStatusLabel->setText(tr("No DBC loaded"));
    m_dbcStatusLabel->setStyleSheet("color: gray; font-style: italic;");
    m_dbcClearBtn->setEnabled(false);
}

void CANConfigWidget::onDBCLoadFinished(int channelIndex, bool success, const QString& errorMsg)
{
    if (channelIndex != m_channelIndex)
        return;

    m_dbcLoadBtn->setEnabled(true);

    if (success) {
        auto db = DBCManager::DBCDatabaseManager::instance().database(channelIndex);
        m_dbcStatusLabel->setText(
            QString("Loaded: %1 messages, %2 signals")
                .arg(db ? db->messages.size() : 0)
                .arg(db ? db->totalSignalCount() : 0));
        m_dbcStatusLabel->setStyleSheet("color: green; font-weight: bold;");
        m_dbcClearBtn->setEnabled(true);
    } else {
        m_dbcStatusLabel->setText(tr("Error: %1").arg(errorMsg));
        m_dbcStatusLabel->setStyleSheet("color: red;");
        m_dbcClearBtn->setEnabled(false);
    }
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

    // Restore Vector channel mapping selection
    if (cfg.interfaceType == "Vector" && cfg.vectorChannelIdx >= 0) {
        // Try to find the matching channel in the combo
        for (int i = 0; i < m_channelMappingCombo->count(); ++i) {
            int idx = m_channelMappingCombo->itemData(i).toInt();
            if (idx >= 0 && idx < m_detectedChannels.size()) {
                const auto& ch = m_detectedChannels[idx];
                if (ch.hwType == cfg.vectorHwType &&
                    ch.hwIndex == cfg.vectorHwIndex &&
                    ch.hwChannel == cfg.vectorHwChannel) {
                    m_channelMappingCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
    }

    // Restore DBC file path
    if (!cfg.dbcFilePath.isEmpty()) {
        m_dbcPathEdit->setText(cfg.dbcFilePath);
    }
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

    // Store Vector channel mapping if a valid channel is selected
    if (cfg.interfaceType == "Vector") {
        int comboIdx = m_channelMappingCombo->currentIndex();
        QVariant userData = m_channelMappingCombo->itemData(comboIdx);
        if (userData.isValid()) {
            int chIdx = userData.toInt();
            if (chIdx >= 0 && chIdx < m_detectedChannels.size()) {
                const auto& ch = m_detectedChannels[chIdx];
                cfg.vectorHwType       = ch.hwType;
                cfg.vectorHwIndex      = ch.hwIndex;
                cfg.vectorHwChannel    = ch.hwChannel;
                cfg.vectorChannelIdx   = ch.channelIndex;
                cfg.vectorChannelMask  = ch.channelMask;
                cfg.device             = ch.name;
                cfg.channel            = ch.hwChannel + 1;
            }
        }
    }

    // Store DBC file path from the widget
    cfg.dbcFilePath = m_dbcPathEdit->text();

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
