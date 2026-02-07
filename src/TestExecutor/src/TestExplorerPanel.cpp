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
#include <QStyle>
#include <QRegularExpression>
#include <functional>

namespace TestExecutor {

namespace {

QString nextSequentialName(const QStringList& existingNames, const QString& prefix)
{
    int maxIndex = 0;
    const QRegularExpression pattern(QString("^%1\\s+(\\d+)$").arg(QRegularExpression::escape(prefix)));
    for (const QString& name : existingNames) {
        const QRegularExpressionMatch match = pattern.match(name.trimmed());
        if (match.hasMatch()) {
            maxIndex = qMax(maxIndex, match.captured(1).toInt());
        }
    }
    return QString("%1 %2").arg(prefix).arg(maxIndex + 1);
}

QString nextUniqueTestId()
{
    QString baseId = TestCase::generateId("TC");
    QString candidate = baseId;
    int suffix = 1;
    auto& repo = TestRepository::instance();
    while (repo.containsTestCase(candidate)) {
        candidate = QString("%1_%2").arg(baseId).arg(suffix++);
    }
    return candidate;
}

QStringList testNamesInBucket(const QString& groupName, const QString& featureName)
{
    QStringList names;
    const auto allTests = TestRepository::instance().allTestCases();
    for (const auto& tc : allTests) {
        if (tc.component.compare(groupName, Qt::CaseInsensitive) == 0 &&
            tc.feature.compare(featureName, Qt::CaseInsensitive) == 0) {
            names.append(tc.name);
        }
    }
    return names;
}

} // namespace

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

    m_btnImport = new QPushButton(this);
    m_btnImport->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_btnImport->setToolTip("Import test cases from JSON file");
    m_btnImport->setFixedSize(28, 28);

    m_btnExport = new QPushButton(this);
    m_btnExport->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_btnExport->setToolTip("Save tests");
    m_btnExport->setFixedSize(28, 28);

    m_btnAddGroup = new QPushButton(this);
    m_btnAddGroup->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    m_btnAddGroup->setToolTip("Add Group");
    m_btnAddGroup->setFixedSize(28, 28);

    m_btnAddFeature = new QPushButton(this);
    m_btnAddFeature->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    m_btnAddFeature->setToolTip("Add Feature");
    m_btnAddFeature->setFixedSize(28, 28);

    m_btnAddTest = new QPushButton(this);
    m_btnAddTest->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    m_btnAddTest->setToolTip("Add Test");
    m_btnAddTest->setFixedSize(28, 28);

    m_btnRemove = new QPushButton(this);
    m_btnRemove->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    m_btnRemove->setToolTip("Remove selected item(s)");
    m_btnRemove->setFixedSize(28, 28);

    m_btnExpand = new QPushButton(this);
    m_btnExpand->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    m_btnExpand->setToolTip("Expand all");
    m_btnExpand->setFixedSize(28, 28);

    m_btnCollapse = new QPushButton(this);
    m_btnCollapse->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    m_btnCollapse->setToolTip("Collapse all");
    m_btnCollapse->setFixedSize(28, 28);

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
    m_treeView->setAlternatingRowColors(false);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragDropMode(QAbstractItemView::InternalMove);
    m_treeView->setStyleSheet("QTreeView::item:selected { background-color: #1F73D2; color: white; }");

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
                copy.id = nextUniqueTestId();
                copy.name += " (Copy)";
                TestRepository::instance().addTestCase(copy);
            }
        }
    });

    m_contextMenu->addSeparator();

    auto* removeAction = m_contextMenu->addAction("Remove");
    connect(removeAction, &QAction::triggered, this, &TestExplorerPanel::onRemoveTestClicked);

    m_contextMenu->addSeparator();

    auto* saveAsAction = m_contextMenu->addAction("Save As...");
    connect(saveAsAction, &QAction::triggered, this, &TestExplorerPanel::onExportAsClicked);
}

void TestExplorerPanel::onImportClicked()
{
    auto& repo = TestRepository::instance();
    if (repo.hasUnsavedChanges()) {
        const int result = QMessageBox::question(
            this,
            "Unsaved Changes",
            "Discard unsaved changes and open another JSON file?",
            QMessageBox::Yes | QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }
    }

    QString filePath = QFileDialog::getOpenFileName(
        this, "Open Test Cases", QString(),
        "Test Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
        return;
    }

    if (repo.loadFromFile(filePath)) {
        QMessageBox::information(this,
                                 "Open Complete",
                                 QString("Loaded %1 test case(s).").arg(repo.testCaseCount()));
        m_treeView->expandAll();
    } else {
        QMessageBox::critical(this, "Open Failed", "Failed to open selected JSON file.");
    }
}

void TestExplorerPanel::onExportClicked()
{
    auto& repo = TestRepository::instance();
    QString filePath = repo.currentFilePath();
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(
            this,
            "Save Test Cases",
            "test_cases.json",
            "Test Files (*.json);;All Files (*)");
        if (filePath.isEmpty()) {
            return;
        }
    }

    if (repo.saveToFile(filePath)) {
        QMessageBox::information(this, "Saved",
                                 QString("Saved %1 test case(s).").arg(repo.testCaseCount()));
    } else {
        QMessageBox::critical(this, "Save Failed", "Failed to save test cases.");
    }
}

void TestExplorerPanel::onExportAsClicked()
{
    auto& repo = TestRepository::instance();
    const QString defaultPath = repo.currentFilePath().isEmpty()
        ? QString("test_cases.json")
        : repo.currentFilePath();

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Test Cases As",
        defaultPath,
        "Test Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
        return;
    }

    if (repo.saveToFile(filePath)) {
        QMessageBox::information(this, "Saved",
                                 QString("Saved %1 test case(s).").arg(repo.testCaseCount()));
    } else {
        QMessageBox::critical(this, "Save Failed", "Failed to save test cases.");
    }
}

void TestExplorerPanel::onAddGroupClicked()
{
    auto& repo = TestRepository::instance();
    const QString groupName = nextSequentialName(repo.allGroups(), "Group");
    repo.addGroup(groupName);

    m_treeView->expandAll();
}

void TestExplorerPanel::onAddFeatureClicked()
{
    auto& repo = TestRepository::instance();

    QString selectedGroup;
    QString featureNameFromSelection;
    resolveSelectionContext(selectedGroup, featureNameFromSelection);

    QString groupName = selectedGroup;
    if (groupName.isEmpty()) {
        groupName = nextSequentialName(repo.allGroups(), "Group");
        repo.addGroup(groupName);
    }

    const QString featureName = nextSequentialName(repo.allFeatures(groupName), "Feature");
    repo.addFeature(groupName, featureName);

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
    newTest.id = nextUniqueTestId();
    newTest.name = nextSequentialName(testNamesInBucket(groupName, featureName), "Test");
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
    auto* model = TestRepository::instance().treeModel();
    const QModelIndexList selected = m_treeView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }

    QSet<QString> groupsToRemove;
    QSet<QString> featuresToRemove;
    QStringList ids = selectedTestCaseIds();
    ids.removeDuplicates();

    for (const QModelIndex& proxyIndex : selected) {
        const QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        const TreeItemType type = model->itemType(sourceIndex);

        if (type == TreeItemType::Group) {
            groupsToRemove.insert(model->itemName(sourceIndex));
        } else if (type == TreeItemType::Feature) {
            const QModelIndex parent = sourceIndex.parent();
            if (parent.isValid()) {
                const QString group = model->itemName(parent);
                const QString feature = model->itemName(sourceIndex);
                featuresToRemove.insert(group + "\x1f" + feature);
            }
        }
    }

    const int result = QMessageBox::question(
        this, "Remove Items",
        "Remove selected group/feature/test items?",
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        auto& repo = TestRepository::instance();

        for (const QString& group : groupsToRemove) {
            repo.removeGroup(group);
        }

        for (const QString& key : featuresToRemove) {
            const QStringList parts = key.split("\x1f");
            if (parts.size() != 2) {
                continue;
            }
            if (groupsToRemove.contains(parts[0])) {
                continue;
            }
            repo.removeFeature(parts[0], parts[1]);
        }

        for (const QString& id : ids) {
            repo.removeTestCase(id);
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
    const QModelIndex index = m_treeView->indexAt(pos);
    if (index.isValid()) {
        if (!m_treeView->selectionModel()->isSelected(index)) {
            m_treeView->selectionModel()->select(index,
                                                 QItemSelectionModel::ClearAndSelect |
                                                 QItemSelectionModel::Rows);
            m_treeView->setCurrentIndex(index);
        }
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
