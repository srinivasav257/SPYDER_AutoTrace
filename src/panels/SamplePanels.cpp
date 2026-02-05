#include "SamplePanels.h"
#include <PanelRegistry.h>

#include <QLabel>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QFont>
#include <QFontDatabase>
#include <QMap>
#include <QVector>
#include <QPair>

// ---------------------------------------------------------------------------
// Helper: creates a QTreeWidget for Test Explorer
// ---------------------------------------------------------------------------
static QWidget *makeTreePanel(QWidget *parent, const QString &title,
                              const QMap<QString, QStringList> &data)
{
    auto *tree = new QTreeWidget(parent);
    tree->setHeaderLabel(title);
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        auto *root = new QTreeWidgetItem(tree, {it.key()});
        for (const auto &childText : it.value()) {
            new QTreeWidgetItem(root, {childText});
        }
        root->setExpanded(true);
    }
    return tree;
}

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
// Helper: creates a read-only QPlainTextEdit for output-style panels
// ---------------------------------------------------------------------------
static QWidget *makeOutputPanel(QWidget *parent, const QString &placeholder)
{
    auto *edit = new QPlainTextEdit(parent);
    edit->setReadOnly(true);
    edit->setPlaceholderText(placeholder);
    // Use a monospaced font for logs
    edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    return edit;
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

    // ==============================================================================
    // Group: TestExecutor
    // ==============================================================================

    // Tests Explorer
    reg.registerPanel({"tests_explorer", "Tests Explorer", "TestExecutor",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           QMap<QString, QStringList> tests;
                           tests["Sanity Checks"] = {"PowerOn_Test", "Communication_Test", "Heartbeat_Test"};
                           tests["Functional Tests"] = {"Brake_System_Test", "Acceleration_Profile", "Emergency_Stop"};
                           tests["Performance"] = {"Long_Run_Stress", "High_Load_Data"};
                           
                           return makeTreePanel(p, "Test Suites", tests);
                       }});


    // Test Editor
    reg.registerPanel({"test_editor", "Test Editor", "TestExecutor",
                       ads::CenterDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = new QTextEdit(p);
                           edit->setPlaceholderText("// Write your test script here...\n\nfunction test() {\n    return true;\n}");
                           edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
                           return edit;
                       }});

    // Progress Window
    reg.registerPanel({"progress_window", "Progress Window", "TestExecutor",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = (QPlainTextEdit*)makeOutputPanel(p, "");
                           edit->appendHtml("<b>Ready to execute tests.</b>");
                           edit->appendPlainText("Waiting for user command...");
                           return edit;
                       }});
}
