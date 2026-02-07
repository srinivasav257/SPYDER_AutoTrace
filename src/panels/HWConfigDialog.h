#pragma once

#include "HWConfigManager.h"
#include <CANManager.h>
#include <DBCManager.h>
#include <QDialog>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <array>

// ---------------------------------------------------------------------------
// Reusable serial port configuration widget
// ---------------------------------------------------------------------------

class SerialConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialConfigWidget(QWidget* parent = nullptr);

    void setConfig(const SerialManager::SerialPortConfig& config);
    SerialManager::SerialPortConfig config() const;

    void refreshPorts();

private:
    QComboBox* m_portCombo;
    QComboBox* m_baudCombo;
    QComboBox* m_dataBitsCombo;
    QComboBox* m_parityCombo;
    QComboBox* m_stopBitsCombo;
    QComboBox* m_flowControlCombo;
    QPushButton* m_refreshBtn;
};

// ---------------------------------------------------------------------------
// CAN port configuration widget
// ---------------------------------------------------------------------------

class CANConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CANConfigWidget(QWidget* parent = nullptr);

    void setConfig(const CANPortConfig& config);
    CANPortConfig config() const;

    void setConnectionStatus(bool connected, const QString& message = {});

    /** @brief Refresh detected Vector hardware channels in the mapping combo. */
    void refreshVectorChannels();

    /** @brief Set the CAN channel index (0 or 1) for DBC association. */
    void setChannelIndex(int index);

signals:
    void connectRequested();
    void disconnectRequested();

private slots:
    void onInterfaceTypeChanged(const QString& type);
    void onLoadDBCClicked();
    void onClearDBCClicked();
    void onDBCLoadFinished(int channelIndex, bool success, const QString& errorMsg);

private:
    QLineEdit* m_aliasEdit;
    QComboBox* m_interfaceTypeCombo;
    QComboBox* m_channelMappingCombo;    ///< Vector HW channel selection
    QPushButton* m_detectHWBtn;          ///< Refresh Vector hardware detection
    QWidget* m_channelMappingRow;        ///< Container for channel mapping (shown for Vector)
    QLineEdit* m_deviceEdit;
    QWidget* m_deviceRow;                ///< Container for manual device field (hidden for Vector)
    QSpinBox* m_channelSpin;
    QWidget* m_channelRow;               ///< Container for manual channel field (hidden for Vector)
    QComboBox* m_bitrateCombo;
    QCheckBox* m_fdEnabledCheck;
    QComboBox* m_fdBitrateCombo;
    QPushButton* m_connectBtn;
    QPushButton* m_disconnectBtn;
    QLabel* m_statusLabel;

    // DBC file association
    int m_channelIndex = -1;                       ///< CAN channel index (0 or 1)
    QLineEdit* m_dbcPathEdit = nullptr;            ///< DBC file path display
    QPushButton* m_dbcLoadBtn = nullptr;            ///< Browse for DBC file
    QPushButton* m_dbcClearBtn = nullptr;           ///< Clear/unload DBC
    QLabel* m_dbcStatusLabel = nullptr;             ///< DBC load status

    QList<CANManager::CANChannelInfo> m_detectedChannels;  ///< Cached detection results
};

// ---------------------------------------------------------------------------
// HW Configuration Dialog
// ---------------------------------------------------------------------------

class HWConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HWConfigDialog(QWidget* parent = nullptr);

private slots:
    void onApply();
    void onOk();

private:
    void setupSerialDebugTab(QTabWidget* parent);
    void setupCANTab(QTabWidget* parent);
    void setupPowerSupplyTab(QTabWidget* parent);
    void setupModbusTab(QTabWidget* parent);

    void loadFromManager();
    void saveToManager();

    // Serial Debug: 4 sub-tabs, each has customName + SerialConfigWidget
    struct SerialDebugTab {
        QLineEdit* customName = nullptr;
        SerialConfigWidget* serialConfig = nullptr;
        QPushButton* connectBtn = nullptr;
        QPushButton* disconnectBtn = nullptr;
        QLabel* statusLabel = nullptr;
    };
    std::array<SerialDebugTab, HWConfigManager::SERIAL_PORT_COUNT> m_serialDebugTabs;

    // CAN: 2 sub-tabs
    std::array<CANConfigWidget*, HWConfigManager::CAN_PORT_COUNT> m_canTabs{};

    // Power Supply
    QLineEdit* m_powerSupplyName = nullptr;
    SerialConfigWidget* m_powerSupplySerial = nullptr;
    QPushButton* m_powerSupplyConnectBtn = nullptr;
    QPushButton* m_powerSupplyDisconnectBtn = nullptr;
    QLabel* m_powerSupplyStatusLabel = nullptr;

    // Modbus Relay
    QLineEdit* m_modbusName = nullptr;
    SerialConfigWidget* m_modbusSerial = nullptr;
    QSpinBox* m_modbusAddress = nullptr;
    QPushButton* m_modbusConnectBtn = nullptr;
    QPushButton* m_modbusDisconnectBtn = nullptr;
    QLabel* m_modbusStatusLabel = nullptr;
};
