#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>

// Forward-declare the config struct to avoid pulling in the full SerialManager header
namespace SerialManager { struct SerialPortConfig; }

/**
 * @brief Reusable serial-port configuration widget.
 *
 * Presents COM port, baud rate, data bits, parity, stop bits and flow-control
 * in a compact QFormLayout.  Used by HWConfigDialog for every serial channel
 * (debug ports, power supply, Modbus relay).
 */
class SerialConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialConfigWidget(QWidget* parent = nullptr);

    void setConfig(const SerialManager::SerialPortConfig& config);
    SerialManager::SerialPortConfig config() const;

    void refreshPorts();

private:
    QComboBox*   m_portCombo;
    QComboBox*   m_baudCombo;
    QComboBox*   m_dataBitsCombo;
    QComboBox*   m_parityCombo;
    QComboBox*   m_stopBitsCombo;
    QComboBox*   m_flowControlCombo;
    QPushButton* m_refreshBtn;
};
