#pragma once

#include "HWConfigManager.h"        // CANPortConfig
#include <CANManager.h>              // CANChannelInfo

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QList>

/**
 * @brief CAN port configuration widget.
 *
 * Presents interface type (Vector/PEAK/SocketCAN), channel mapping (auto-detected
 * for Vector hardware), bit rate, CAN-FD settings, and DBC file association.
 */
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
    QLineEdit*   m_aliasEdit              = nullptr;
    QComboBox*   m_interfaceTypeCombo     = nullptr;
    QComboBox*   m_channelMappingCombo    = nullptr;  ///< Vector HW channel selection
    QPushButton* m_detectHWBtn            = nullptr;  ///< Refresh Vector hardware detection
    QWidget*     m_channelMappingRow      = nullptr;  ///< Container for channel mapping (shown for Vector)
    QLineEdit*   m_deviceEdit             = nullptr;
    QWidget*     m_deviceRow              = nullptr;  ///< Container for manual device field (hidden for Vector)
    QSpinBox*    m_channelSpin            = nullptr;
    QWidget*     m_channelRow             = nullptr;  ///< Container for manual channel field (hidden for Vector)
    QComboBox*   m_bitrateCombo           = nullptr;
    QCheckBox*   m_fdEnabledCheck         = nullptr;
    QComboBox*   m_fdBitrateCombo         = nullptr;
    QPushButton* m_connectBtn             = nullptr;
    QPushButton* m_disconnectBtn          = nullptr;
    QLabel*      m_statusLabel            = nullptr;

    // DBC file association
    int          m_channelIndex = -1;                          ///< CAN channel index (0 or 1)
    QLineEdit*   m_dbcPathEdit   = nullptr;                    ///< DBC file path display
    QPushButton* m_dbcLoadBtn    = nullptr;                    ///< Browse for DBC file
    QPushButton* m_dbcClearBtn   = nullptr;                    ///< Clear/unload DBC
    QLabel*      m_dbcStatusLabel = nullptr;                   ///< DBC load status

    QList<CANManager::CANChannelInfo> m_detectedChannels;      ///< Cached detection results
};
