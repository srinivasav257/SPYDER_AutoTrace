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
#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>

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
                           auto *container = new QWidget(p);
                           auto *mainLayout = new QVBoxLayout(container); // Main container layout
                           mainLayout->setContentsMargins(0,0,0,0);

                           // Main Vertical Splitter
                           auto *splitter = new QSplitter(Qt::Vertical, container);
                           mainLayout->addWidget(splitter);

                           // =========================================================
                           // TOP PANE: Case Properties + Toolbar + Execution Sequence
                           // =========================================================
                           auto *topPane = new QWidget(splitter);
                           auto *topLayout = new QVBoxLayout(topPane);
                           
                           // 1. Test Case Properties
                           auto *metaGroup = new QGroupBox("Test Case Properties", topPane);
                           auto *metaLayout = new QFormLayout(metaGroup);
                           metaLayout->addRow("Name:", new QLineEdit("TestCase_01_Enter_MD_Session", metaGroup));
                           metaLayout->addRow("Description:", new QLineEdit("Verify entry into Manual Diagnostic session.", metaGroup));
                           
                           topLayout->addWidget(metaGroup);

                           // 2. Toolbar (Add/Remove Steps)
                           auto *toolbarLayout = new QHBoxLayout();
                           auto *btnAddStep = new QPushButton("Add Step", topPane);
                           auto *btnRemoveStep = new QPushButton("Remove Step", topPane);
                           toolbarLayout->addWidget(btnAddStep);
                           toolbarLayout->addWidget(btnRemoveStep);
                           toolbarLayout->addStretch(); // Push buttons to left
                           topLayout->addLayout(toolbarLayout);

                           // 3. Execution Sequence Table
                           auto *seqGroup = new QGroupBox("Execution Sequence", topPane);
                           auto *seqBoxLayout = new QVBoxLayout(seqGroup);
                           auto *seqTable = new QTableWidget(4, 4, seqGroup); // 4 Columns
                           seqTable->setHorizontalHeaderLabels({"#", "Type", "Command", "Parameters"});
                           seqTable->horizontalHeader()->setStretchLastSection(true);
                           seqTable->setSelectionBehavior(QAbstractItemView::SelectRows);
                           // seqTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Allow editing for ComboBoxes
                           seqTable->verticalHeader()->setVisible(false);

                           // Helper lambda to add a row with logic
                           auto addRow = [seqTable](int row, const QString& num, const QString& type, const QString& cmd, const QString& params) {
                               seqTable->setItem(row, 0, new QTableWidgetItem(num));
                               
                               // Type ComboBox
                               auto *typeCombo = new QComboBox();
                               typeCombo->addItems({"ManDiag", "Power", "CAN", "Flow"});
                               typeCombo->setCurrentText(type);
                               seqTable->setCellWidget(row, 1, typeCombo);

                               // Command ComboBox
                               auto *cmdCombo = new QComboBox();
                               seqTable->setCellWidget(row, 2, cmdCombo);

                               // Parameters Item
                               seqTable->setItem(row, 3, new QTableWidgetItem(params));

                               // Logic to update Commands based on Type
                               auto updateCmds = [typeCombo, cmdCombo](const QString& t) {
                                   cmdCombo->clear();
                                   if (t == "ManDiag") {
                                       cmdCombo->addItems({"Enter_MD_Session", "Exit_MD_Session", "Read_DID", "Write_DID"});
                                   } else if (t == "Power") {
                                       cmdCombo->addItems({"Turn_ON_PPS", "Turn_OFF_PPS", "Set_Voltage", "Read_Current"});
                                   } else if (t == "CAN") {
                                       cmdCombo->addItems({"SEND_CAN", "CHECK_VAL", "READ_MSG"});
                                   } else if (t == "Flow") {
                                       cmdCombo->addItems({"WAIT", "LOOP", "IF_CONDITION"});
                                   }
                               };

                               // Connect change signal
                               QObject::connect(typeCombo, &QComboBox::currentTextChanged, updateCmds);
                               
                               // Initial population
                               updateCmds(type);
                               cmdCombo->setCurrentText(cmd);
                           };

                           // Populate Data
                           addRow(0, "1", "Power", "Turn_ON_PPS", "12.0V");
                           addRow(1, "2", "Flow", "WAIT", "500ms");
                           addRow(2, "3", "CAN", "SEND_CAN", "ID: 0x100");
                           addRow(3, "4", "Flow", "WAIT", "100ms");

                           seqBoxLayout->addWidget(seqTable);
                           topLayout->addWidget(seqGroup);

                           // =========================================================
                           // BOTTOM PANE: Step Configuration
                           // =========================================================
                           auto *bottomPane = new QWidget(splitter);
                           auto *bottomLayout = new QVBoxLayout(bottomPane);
                           auto *propGroup = new QGroupBox("Step Configuration", bottomPane);
                           auto *propLayout = new QVBoxLayout(propGroup);
                           auto *stack = new QStackedWidget(propGroup);
                           propLayout->addWidget(stack);
                           bottomLayout->addWidget(propGroup);

                           // --- Page 0: Default/Empty ---
                           auto *pageEmpty = new QLabel("Select a step to edit.", stack);
                           pageEmpty->setAlignment(Qt::AlignCenter);
                           stack->addWidget(pageEmpty);

                           // --- Page 1: WAIT Editor ---
                           auto *pageWait = new QWidget(stack);
                           auto *formWait = new QFormLayout(pageWait);
                           auto *spinTime = new QSpinBox(pageWait);
                           spinTime->setRange(0, 60000); spinTime->setSuffix(" ms");
                           formWait->addRow("Wait Duration:", spinTime);
                           formWait->addRow(new QLabel("Pauses execution for the specified time.", pageWait));
                           stack->addWidget(pageWait);

                           // --- Page 2: SEND_CAN Editor ---
                           auto *pageCan = new QWidget(stack);
                           auto *formCan = new QFormLayout(pageCan);
                           auto *spinId = new QSpinBox(pageCan);
                           spinId->setDisplayIntegerBase(16); spinId->setPrefix("0x"); spinId->setRange(0, 0x1FFFFFFF);
                           auto *editData = new QLineEdit("00 00 00 00", pageCan);
                           formCan->addRow("Message ID:", spinId);
                           formCan->addRow("Payload:", editData);
                           formCan->addRow("DLC:", new QSpinBox(pageCan));
                           stack->addWidget(pageCan);

                           // --- Page 3: PPS Editor ---
                           auto *pagePps = new QWidget(stack);
                           auto *formPps = new QFormLayout(pagePps);
                           auto *spinVolt = new QDoubleSpinBox(pagePps);
                           spinVolt->setSuffix(" V");
                           formPps->addRow("Voltage:", spinVolt);
                           formPps->addRow("Current Limit:", new QDoubleSpinBox(pagePps));
                           stack->addWidget(pagePps);

                           // --- LOGIC: Connect Selection to Stack Index ---
                           QObject::connect(seqTable, &QTableWidget::itemSelectionChanged, [seqTable, stack, spinTime, spinId, spinVolt]() {
                               auto items = seqTable->selectedItems();
                               if (items.isEmpty()) {
                                   stack->setCurrentIndex(0); // Empty
                                   return;
                               }
                               
                               // Get Command String from Column 2 (Command ComboBox)
                               int row = items.first()->row();
                               auto *cmdCombo = qobject_cast<QComboBox*>(seqTable->cellWidget(row, 2));
                               if (!cmdCombo) return;
                               QString cmd = cmdCombo->currentText();

                               // Update Editor based on Command
                               if (cmd == "WAIT") {
                                   stack->setCurrentIndex(1);
                                   auto *pItem = seqTable->item(row, 3);
                                   if (pItem) {
                                       QString val = pItem->text().replace("ms","");
                                       spinTime->setValue(val.toInt());
                                   }
                               }
                               else if (cmd == "SEND_CAN") {
                                   stack->setCurrentIndex(2);
                                   spinId->setValue(0x100);
                               }
                               else if (cmd == "Turn_ON_PPS") {
                                   stack->setCurrentIndex(3);
                                   spinVolt->setValue(12.0);
                               }
                               else {
                                   stack->setCurrentIndex(0); 
                               }
                           });
                           
                           // Add widgets to splitter
                           splitter->addWidget(topPane);
                           splitter->addWidget(bottomPane);
                           splitter->setStretchFactor(0, 3); // Give more space to sequence
                           splitter->setStretchFactor(1, 1);

                           return container;
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
