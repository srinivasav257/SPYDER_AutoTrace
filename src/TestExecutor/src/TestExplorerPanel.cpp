/**
 * @file TestExplorerPanel.cpp
 * @brief Implementation of Test Explorer Panel.
 */

#include "TestExplorerPanel.h"
#include "TestEditorPanel.h"
#include "TestRepository.h"
#include "TestExecutorEngine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QInputDialog>
#include <QStyle>
#include <functional>

namespace TestExecutor {

TestExplorerPanel::TestExplorerPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
    createContextMenu();
}

TestExplorerPanel::~TestExplorerPanel() = default;

QStringList TestExplorerPanel::selectedTestCaseIds() const
{
    QStringList ids;
    QSet<QString> seen;

    const QModelIndexList selected = m_treeView->selectionModel()->selectedRows();
    for (const QModelIndex& proxyIndex : selected) {
        const QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        collectTestCaseIdsFromSourceIndex(sourceIndex, ids, seen);
    }

    return ids;
}

void TestExplorerPanel::selectTestCase(const QString& id)
{
    auto* model = TestRepository::instance().treeModel();

    // Find the index for this id
    std::function<QModelIndex(const QModelIndex&)> findIndex;
    findIndex = [&](const QModelIndex& parent) -> QModelIndex {
        const int rows = model->rowCount(parent);
        for (int i = 0; i < rows; ++i) {
            QModelIndex idx = model->index(i, 0, parent);
            if (model->itemId(idx) == id) {
                return idx;
            }
            QModelIndex child = findIndex(idx);
            if (child.isValid()) {
                return child;
            }
        }
        return QModelIndex();
    };

    QModelIndex sourceIndex = findIndex(QModelIndex());
    if (sourceIndex.isValid()) {
        QModelIndex proxyIndex = m_proxyModel->mapFromSource(sourceIndex);
        m_treeView->selectionModel()->select(proxyIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        m_treeView->scrollTo(proxyIndex);
    }
}

void TestExplorerPanel::expandAll()
{
    m_treeView->expandAll();
}

void TestExplorerPanel::collapseAll()
{
    m_treeView->collapseAll();
}

void TestExplorerPanel::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // === Toolbar ===
    auto* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(2);

    m_btnImport = new QPushButton("Import", this);
    m_btnImport->setToolTip("Import test cases from JSON file");
    m_btnImport->setMaximumWidth(60);

    m_btnExport = new QPushButton("Export", this);
    m_btnExport->setToolTip("Export selected test cases to JSON file");
    m_btnExport->setMaximumWidth(60);

    m_btnAddGroup = new QPushButton(this);
    m_btnAddGroup->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    m_btnAddGroup->setText("+G");
    m_btnAddGroup->setToolTip("Add Group");
    m_btnAddGroup->setMaximumWidth(50);

    m_btnAddFeature = new QPushButton(this);
    m_btnAddFeature->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_btnAddFeature->setText("+F");
    m_btnAddFeature->setToolTip("Add Feature");
    m_btnAddFeature->setMaximumWidth(50);

    m_btnAddTest = new QPushButton(this);
    m_btnAddTest->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    m_btnAddTest->setText("+T");
    m_btnAddTest->setToolTip("Add Test");
    m_btnAddTest->setMaximumWidth(50);

    m_btnRemove = new QPushButton("-", this);
    m_btnRemove->setToolTip("Remove selected test case");
    m_btnRemove->setMaximumWidth(30);

    m_btnExpand = new QPushButton("Expand", this);
    m_btnExpand->setToolTip("Expand all");
    m_btnExpand->setMaximumWidth(65);

    m_btnCollapse = new QPushButton("Collapse", this);
    m_btnCollapse->setToolTip("Collapse all");
    m_btnCollapse->setMaximumWidth(65);

    m_btnRun = new QPushButton("Run", this);
    m_btnRun->setToolTip("Run checked or selected tests");
    m_btnRun->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");

    toolbarLayout->addWidget(m_btnImport);
    toolbarLayout->addWidget(m_btnExport);
    toolbarLayout->addWidget(m_btnAddGroup);
    toolbarLayout->addWidget(m_btnAddFeature);
    toolbarLayout->addWidget(m_btnAddTest);
    toolbarLayout->addWidget(m_btnRemove);
    toolbarLayout->addSpacing(10);
    toolbarLayout->addWidget(m_btnExpand);
    toolbarLayout->addWidget(m_btnCollapse);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_btnRun);

    layout->addLayout(toolbarLayout);

    // === Search Box ===
    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search tests...");
    m_searchBox->setClearButtonEnabled(true);
    layout->addWidget(m_searchBox);

    // === Tree View ===
    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(false);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragDropMode(QAbstractItemView::InternalMove);

    // Setup proxy model for filtering
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setRecursiveFilteringEnabled(true);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(0);
    m_proxyModel->setSourceModel(TestRepository::instance().treeModel());

    m_treeView->setModel(m_proxyModel);
    m_treeView->expandAll();
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    layout->addWidget(m_treeView);
}

void TestExplorerPanel::setupConnections()
{
    connect(m_btnImport, &QPushButton::clicked, this, &TestExplorerPanel::onImportClicked);
    connect(m_btnExport, &QPushButton::clicked, this, &TestExplorerPanel::onExportClicked);
    connect(m_btnAddGroup, &QPushButton::clicked, this, &TestExplorerPanel::onAddGroupClicked);
    connect(m_btnAddFeature, &QPushButton::clicked, this, &TestExplorerPanel::onAddFeatureClicked);
    connect(m_btnAddTest, &QPushButton::clicked, this, &TestExplorerPanel::onAddTestClicked);
    connect(m_btnRemove, &QPushButton::clicked, this, &TestExplorerPanel::onRemoveTestClicked);
    connect(m_btnExpand, &QPushButton::clicked, this, &TestExplorerPanel::expandAll);
    connect(m_btnCollapse, &QPushButton::clicked, this, &TestExplorerPanel::collapseAll);
    connect(m_btnRun, &QPushButton::clicked, this, &TestExplorerPanel::onRunSelectedClicked);

    connect(m_searchBox, &QLineEdit::textChanged, this, &TestExplorerPanel::onSearchTextChanged);

    connect(m_treeView, &QTreeView::doubleClicked, this, &TestExplorerPanel::onTreeDoubleClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &TestExplorerPanel::onTreeContextMenu);

    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() {
                emit selectionChanged(selectedTestCaseIds());
            });
}

void TestExplorerPanel::createContextMenu()
{
    m_contextMenu = new QMenu(this);

    auto* runAction = m_contextMenu->addAction("Run Checked/Selected");
    connect(runAction, &QAction::triggered, this, &TestExplorerPanel::onRunSelectedClicked);

    m_contextMenu->addSeparator();

    auto* addGroupAction = m_contextMenu->addAction("Add Group");
    connect(addGroupAction, &QAction::triggered, this, &TestExplorerPanel::onAddGroupClicked);

    auto* addFeatureAction = m_contextMenu->addAction("Add Feature");
    connect(addFeatureAction, &QAction::triggered, this, &TestExplorerPanel::onAddFeatureClicked);

    auto* addTestAction = m_contextMenu->addAction("Add Test");
    connect(addTestAction, &QAction::triggered, this, &TestExplorerPanel::onAddTestClicked);

    m_contextMenu->addSeparator();

    auto* editAction = m_contextMenu->addAction("Edit Test");
    connect(editAction, &QAction::triggered, this, [this]() {
        QStringList ids = selectedTestCaseIds();
        if (!ids.isEmpty()) {
            auto* dialog = new TestEditorDialog(this);
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->loadTestCase(ids.first());
            dialog->show();

            emit testCaseDoubleClicked(ids.first());
        }
    });

    auto* duplicateAction = m_contextMenu->addAction("Duplicate");
    connect(duplicateAction, &QAction::triggered, this, [this]() {
        QStringList ids = selectedTestCaseIds();
        for (const QString& id : ids) {
            const TestCase* tc = TestRepository::instance().testCase(id);
            if (tc) {
                TestCase copy = *tc;
                copy.id = TestCase::generateId("TC");
                copy.name += " (Copy)";
                TestRepository::instance().addTestCase(copy);
            }
        }
    });

    m_contextMenu->addSeparator();

    auto* removeAction = m_contextMenu->addAction("Remove");
    connect(removeAction, &QAction::triggered, this, &TestExplorerPanel::onRemoveTestClicked);

    m_contextMenu->addSeparator();

    auto* exportAction = m_contextMenu->addAction("Export Selected...");
    connect(exportAction, &QAction::triggered, this, &TestExplorerPanel::onExportClicked);
}

void TestExplorerPanel::onImportClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Import Test Cases", QString(),
        "Test Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
        return;
    }

    int imported = TestRepository::instance().importFromFile(filePath, false);

    QMessageBox::information(this, "Import Complete",
                             QString("Imported %1 test case(s).").arg(imported));
}

void TestExplorerPanel::onExportClicked()
{
    QStringList ids = selectedTestCaseIds();
    if (ids.isEmpty()) {
        // Export all
        for (const auto& tc : TestRepository::instance().allTestCases()) {
            ids.append(tc.id);
        }
    }

    if (ids.isEmpty()) {
        QMessageBox::warning(this, "Export", "No test cases to export.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, "Export Test Cases", "test_cases.json",
        "Test Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
        return;
    }

    if (TestRepository::instance().exportToFile(ids, filePath)) {
        QMessageBox::information(this, "Export Complete",
                                 QString("Exported %1 test case(s).").arg(ids.size()));
    } else {
        QMessageBox::critical(this, "Export Failed",
                              "Failed to export test cases.");
    }
}

void TestExplorerPanel::onAddGroupClicked()
{
    bool ok = false;
    const QString groupName = QInputDialog::getText(
        this, "Add Group", "Group name:", QLineEdit::Normal, QString(), &ok).trimmed();

    if (!ok || groupName.isEmpty()) {
        return;
    }

    if (!TestRepository::instance().addGroup(groupName)) {
        QMessageBox::information(this, "Group Exists", "That group already exists.");
        return;
    }

    m_treeView->expandAll();
}

void TestExplorerPanel::onAddFeatureClicked()
{
    auto& repo = TestRepository::instance();

    QString selectedGroup;
    QString featureNameFromSelection;
    resolveSelectionContext(selectedGroup, featureNameFromSelection);

    bool ok = false;
    QString groupName = selectedGroup;
    if (groupName.isEmpty()) {
        const QStringList groups = repo.allGroups();
        groupName = QInputDialog::getItem(this,
                                          "Select Group",
                                          "Add feature under group:",
                                          groups,
                                          0,
                                          true,
                                          &ok).trimmed();
        if (!ok || groupName.isEmpty()) {
            return;
        }
    }

    const QString featureName = QInputDialog::getText(
        this, "Add Feature", "Feature name:", QLineEdit::Normal, QString(), &ok).trimmed();

    if (!ok || featureName.isEmpty()) {
        return;
    }

    if (!repo.addFeature(groupName, featureName)) {
        QMessageBox::information(this, "Feature Exists", "That feature already exists in this group.");
        return;
    }

    m_treeView->expandAll();
}

void TestExplorerPanel::onAddTestClicked()
{
    auto& repo = TestRepository::instance();

    QString groupName;
    QString featureName;
    resolveSelectionContext(groupName, featureName);

    if (groupName.isEmpty()) {
        groupName = "Ungrouped";
        repo.addGroup(groupName);
    }

    if (featureName.isEmpty()) {
        featureName = "General";
        repo.addFeature(groupName, featureName);
    }

    TestCase newTest = repo.createNewTestCase();
    newTest.component = groupName;
    newTest.feature = featureName;
    repo.addTestCase(newTest);

    selectTestCase(newTest.id);

    // Open the Test Editor dialog for the new test
    auto* dialog = new TestEditorDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->loadTestCase(newTest.id);
    dialog->show();

    emit testCaseDoubleClicked(newTest.id);
    emit newTestRequested();
}

void TestExplorerPanel::onRemoveTestClicked()
{
    QStringList ids = selectedTestCaseIds();
    if (ids.isEmpty()) {
        return;
    }

    const int result = QMessageBox::question(
        this, "Remove Test Cases",
        QString("Are you sure you want to remove %1 test case(s)?").arg(ids.size()),
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        for (const QString& id : ids) {
            TestRepository::instance().removeTestCase(id);
        }
    }
}

void TestExplorerPanel::onSearchTextChanged(const QString& text)
{
    m_proxyModel->setFilterFixedString(text);

    if (!text.isEmpty()) {
        m_treeView->expandAll();
    }
}

void TestExplorerPanel::onTreeDoubleClicked(const QModelIndex& index)
{
    const QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    auto* model = TestRepository::instance().treeModel();

    if (model->itemType(sourceIndex) == TreeItemType::TestCase) {
        const QString testCaseId = model->itemId(sourceIndex);

        auto* dialog = new TestEditorDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->loadTestCase(testCaseId);
        dialog->show();

        emit testCaseDoubleClicked(testCaseId);
    }
}

void TestExplorerPanel::onTreeContextMenu(const QPoint& pos)
{
    QModelIndex index = m_treeView->indexAt(pos);
    if (index.isValid()) {
        m_contextMenu->exec(m_treeView->viewport()->mapToGlobal(pos));
    }
}

void TestExplorerPanel::onRunSelectedClicked()
{
    auto* model = TestRepository::instance().treeModel();
    QStringList ids = model->checkedTestCaseIds();

    if (ids.isEmpty()) {
        ids = selectedTestCaseIds();
    }
    ids.removeDuplicates();

    if (ids.isEmpty()) {
        QMessageBox::information(this, "Run Tests", "Please check or select test cases to run.");
        return;
    }

    emit runSelectedRequested(ids);

    // Start execution
    TestExecutorEngine::instance().runTests(ids);
}

void TestExplorerPanel::resolveSelectionContext(QString& groupName, QString& featureName) const
{
    groupName.clear();
    featureName.clear();

    const QModelIndex currentProxy = m_treeView->currentIndex();
    if (!currentProxy.isValid()) {
        return;
    }

    QModelIndex sourceIndex = m_proxyModel->mapToSource(currentProxy);
    auto* model = TestRepository::instance().treeModel();

    while (sourceIndex.isValid()) {
        const TreeItemType type = model->itemType(sourceIndex);
        if (type == TreeItemType::Group && groupName.isEmpty()) {
            groupName = model->itemName(sourceIndex);
        } else if (type == TreeItemType::Feature && featureName.isEmpty()) {
            featureName = model->itemName(sourceIndex);
        }

        sourceIndex = sourceIndex.parent();
    }
}

void TestExplorerPanel::collectTestCaseIdsFromSourceIndex(const QModelIndex& sourceIndex,
                                                          QStringList& ids,
                                                          QSet<QString>& seen) const
{
    if (!sourceIndex.isValid()) {
        return;
    }

    auto* model = TestRepository::instance().treeModel();
    if (model->itemType(sourceIndex) == TreeItemType::TestCase) {
        const QString id = model->itemId(sourceIndex);
        if (!id.isEmpty() && !seen.contains(id)) {
            seen.insert(id);
            ids.append(id);
        }
        return;
    }

    const int childCount = model->rowCount(sourceIndex);
    for (int i = 0; i < childCount; ++i) {
        collectTestCaseIdsFromSourceIndex(model->index(i, 0, sourceIndex), ids, seen);
    }
}

QWidget* createTestExplorerPanel(QWidget* parent)
{
    return new TestExplorerPanel(parent);
}

} // namespace TestExecutor
