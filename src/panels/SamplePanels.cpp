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

#include "TestTreeModel.h"
#include <QTreeView>

// ---------------------------------------------------------------------------
// Helper: creates a QTreeView with TestTreeModel
// ---------------------------------------------------------------------------
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

// ---------------------------------------------------------------------------
// Helper: creates a QTreeView with TestTreeModel and Toolbar
// ---------------------------------------------------------------------------
static QWidget *makeTestTreePanel(QWidget *parent)
{
    auto *container = new QWidget(parent);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    auto *toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(2, 2, 2, 2);

    auto *btnAdd = new QPushButton("Add", container);
    auto *btnRemove = new QPushButton("Del", container);
    auto *btnImport = new QPushButton("Imp", container);
    auto *btnExport = new QPushButton("Exp", container);
    auto *btnExpand = new QPushButton("Expand", container);
    auto *btnCollapse = new QPushButton("Collapse", container);
    auto *searchBox = new QLineEdit(container);
    searchBox->setPlaceholderText("Search...");

    toolbarLayout->addWidget(btnAdd);
    toolbarLayout->addWidget(btnRemove);
    toolbarLayout->addWidget(btnImport);
    toolbarLayout->addWidget(btnExport);
    toolbarLayout->addWidget(btnExpand);
    toolbarLayout->addWidget(btnCollapse);
    toolbarLayout->addWidget(searchBox);
    layout->addLayout(toolbarLayout);

    // Tree View
    auto *view = new QTreeView(container);
    auto *model = new TestTreeModel("", view);
    view->setModel(model);
    view->expandAll();
    view->header()->setStretchLastSection(true);
    view->resizeColumnToContents(0);
    layout->addWidget(view);

    // Checkable buttons logic
    QObject::connect(btnExpand, &QPushButton::clicked, view, &QTreeView::expandAll);
    QObject::connect(btnCollapse, &QPushButton::clicked, view, &QTreeView::collapseAll);
    
    // Simple filter placeholder - connecting a SortFilterProxyModel in a single function is tricky 
    // without a class context, but we can set it up here if needed. 
    // For now, highlighting only logic requires custom delegate or proxy.
    // Leaving search as a UI placeholder per user prompt "options... Search for test".
    
    return container;
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
                           return makeTestTreePanel(p);
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
