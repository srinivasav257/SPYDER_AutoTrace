#include "SamplePanels.h"
#include <PanelRegistry.h>

#include <QTableWidget>
#include <QHeaderView>
#include <QVector>
#include <QPair>

// ---------------------------------------------------------------------------
// Helper: creates a QTableWidget for Configuration
// ---------------------------------------------------------------------------
static QWidget *makeConfigTable(QWidget *parent, const QStringList &headers,
                                const QVector<QPair<QString, QString>> &rowsData)
{
    auto *table = new QTableWidget(rowsData.size(), headers.size(), parent);
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);

    for (int r = 0; r < rowsData.size(); ++r)
    {
        table->setItem(r, 0, new QTableWidgetItem(rowsData[r].first));
        table->setItem(r, 1, new QTableWidgetItem(rowsData[r].second));
    }
    return table;
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------
void registerSamplePanels()
{
    auto &reg = DockManager::PanelRegistry::instance();

    // ==============================================================================
    // Group: Configuration
    // ==============================================================================

    // CAN Port Config
    reg.registerPanel({"can_config", "CAN Port Config", "Configuration",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           QVector<QPair<QString, QString>> data;
                           data.append(qMakePair(QString("Interface"), QString("PEAK_CAN")));
                           data.append(qMakePair(QString("Device"), QString("PCAN_USBBUS1")));
                           data.append(qMakePair(QString("Bitrate"), QString("500 kBit/s")));
                           data.append(qMakePair(QString("FD Mode"), QString("Enabled")));
                           data.append(qMakePair(QString("Data Bitrate"), QString("2 MBit/s")));

                           return makeConfigTable(p, {"Parameter", "Value"}, data);
                       }});

    // Serial Port Config
    reg.registerPanel({"serial_config", "Serial Port Config", "Configuration",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           QVector<QPair<QString, QString>> data;
                           data.append(qMakePair(QString("Port"), QString("COM3")));
                           data.append(qMakePair(QString("Baud Rate"), QString("115200")));
                           data.append(qMakePair(QString("Data Bits"), QString("8")));
                           data.append(qMakePair(QString("Parity"), QString("None")));
                           data.append(qMakePair(QString("Stop Bits"), QString("1")));
                           data.append(qMakePair(QString("Flow Control"), QString("None")));

                           return makeConfigTable(p, {"Parameter", "Value"}, data);
                       }});
}
