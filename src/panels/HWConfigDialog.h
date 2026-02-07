#pragma once

#include "HWConfigManager.h"
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

signals:
    void connectRequested();
    void disconnectRequested();

private:
    QLineEdit* m_aliasEdit;
    QComboBox* m_interfaceTypeCombo;
    QLineEdit* m_deviceEdit;
    QSpinBox* m_channelSpin;
    QComboBox* m_bitrateCombo;
    QCheckBox* m_fdEnabledCheck;
    QComboBox* m_fdBitrateCombo;
    QPushButton* m_connectBtn;
    QPushButton* m_disconnectBtn;
    QLabel* m_statusLabel;
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
