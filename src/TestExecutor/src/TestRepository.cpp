/**
 * @file TestRepository.cpp
 * @brief Implementation of Test Repository and Tree Model.
 */

#include "TestRepository.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMimeData>
#include <QDebug>

namespace TestExecutor {

//=============================================================================
// TreeItem - Internal tree structure
//=============================================================================

struct TestTreeModel::TreeItem
{
    QString id;
    QString name;
    QString description;
    TreeItemType type = TreeItemType::Root;
    TreeItem* parent = nullptr;
    QVector<TreeItem*> children;
    
    // For test cases
    TestStatus status = TestStatus::NotRun;
    QString requirementId;
    QString jiraTicket;
    QStringList tags;
    
    ~TreeItem() {
        qDeleteAll(children);
    }
    
    int row() const {
        if (parent) {
            return parent->children.indexOf(const_cast<TreeItem*>(this));
        }
        return 0;
    }
};

//=============================================================================
// TestTreeModel Implementation
//=============================================================================

TestTreeModel::TestTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_rootItem(new TreeItem())
{
    m_rootItem->type = TreeItemType::Root;
    m_rootItem->name = "Root";
}

TestTreeModel::~TestTreeModel() = default;

QModelIndex TestTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    
    TreeItem* parentItem = parent.isValid() 
        ? static_cast<TreeItem*>(parent.internalPointer())
        : m_rootItem.get();
    
    if (row >= 0 && row < parentItem->children.size()) {
        return createIndex(row, column, parentItem->children[row]);
    }
    
    return QModelIndex();
}

QModelIndex TestTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    
    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->parent;
    
    if (!parentItem || parentItem == m_rootItem.get()) {
        return QModelIndex();
    }
    
    return createIndex(parentItem->row(), 0, parentItem);
}

int TestTreeModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem = parent.isValid()
        ? static_cast<TreeItem*>(parent.internalPointer())
        : m_rootItem.get();
    
    return parentItem->children.size();
}

int TestTreeModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4; // Name, Description, Requirement, JIRA
}

QVariant TestTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0: return item->name;
                case 1: return item->description;
                case 2: return item->requirementId;
                case 3: return item->jiraTicket;
            }
            break;
            
        case IdRole:
            return item->id;
            
        case TypeRole:
            return static_cast<int>(item->type);
            
        case StatusRole:
            return static_cast<int>(item->status);
            
        case DescriptionRole:
            return item->description;
            
        case RequirementRole:
            return item->requirementId;
            
        case JiraRole:
            return item->jiraTicket;
            
        case TagsRole:
            return item->tags;
            
        case Qt::DecorationRole:
            if (index.column() == 0) {
                if (item->type == TreeItemType::Suite) {
                    // Return folder icon
                } else if (item->type == TreeItemType::TestCase) {
                    // Return test icon based on status
                }
            }
            break;
    }
    
    return QVariant();
}

QVariant TestTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "Test Name";
            case 1: return "Description";
            case 2: return "Requirement";
            case 3: return "JIRA";
        }
    }
    return QVariant();
}

Qt::ItemFlags TestTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if (item->type == TreeItemType::TestCase) {
        flags |= Qt::ItemIsDragEnabled;
    }
    if (item->type == TreeItemType::Suite || item->type == TreeItemType::Root) {
        flags |= Qt::ItemIsDropEnabled;
    }
    
    return flags;
}

Qt::DropActions TestTreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList TestTreeModel::mimeTypes() const
{
    return {"application/x-testcase-id"};
}

QMimeData* TestTreeModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;
    
    for (const QModelIndex& index : indexes) {
        if (index.isValid() && index.column() == 0) {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            encodedData.append(item->id.toUtf8());
            encodedData.append('\n');
        }
    }
    
    mimeData->setData("application/x-testcase-id", encodedData);
    return mimeData;
}

bool TestTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                                  int /*row*/, int /*column*/, const QModelIndex& /*parent*/)
{
    if (action == Qt::IgnoreAction) {
        return true;
    }
    
    if (!data->hasFormat("application/x-testcase-id")) {
        return false;
    }
    
    // Handle drop - move test case to new suite
    // Implementation depends on TestRepository integration
    
    return false;
}

QString TestTreeModel::itemId(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QString();
    }
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->id;
}

TreeItemType TestTreeModel::itemType(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return TreeItemType::Root;
    }
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->type;
}

TestCase* TestTreeModel::testCaseAt(const QModelIndex& index)
{
    if (!index.isValid()) {
        return nullptr;
    }
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if (item->type != TreeItemType::TestCase) {
        return nullptr;
    }
    return TestRepository::instance().testCase(item->id);
}

TestSuite* TestTreeModel::testSuiteAt(const QModelIndex& index)
{
    if (!index.isValid()) {
        return nullptr;
    }
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if (item->type != TreeItemType::Suite) {
        return nullptr;
    }
    return TestRepository::instance().testSuite(item->id);
}

void TestTreeModel::refresh()
{
    beginResetModel();
    buildTree();
    endResetModel();
}

void TestTreeModel::buildTree()
{
    // Clear existing tree
    qDeleteAll(m_rootItem->children);
    m_rootItem->children.clear();
    
    auto& repo = TestRepository::instance();
    
    // Group by component or suite
    QMap<QString, TreeItem*> componentItems;
    
    for (const auto& tc : repo.allTestCases()) {
        QString component = tc.component.isEmpty() ? "Unsorted" : tc.component;
        
        // Create component/suite node if needed
        if (!componentItems.contains(component)) {
            TreeItem* suiteItem = new TreeItem();
            suiteItem->type = TreeItemType::Suite;
            suiteItem->id = "suite_" + component;
            suiteItem->name = component;
            suiteItem->parent = m_rootItem.get();
            m_rootItem->children.append(suiteItem);
            componentItems[component] = suiteItem;
        }
        
        // Create test case node
        TreeItem* tcItem = new TreeItem();
        tcItem->type = TreeItemType::TestCase;
        tcItem->id = tc.id;
        tcItem->name = tc.name;
        tcItem->description = tc.description;
        tcItem->requirementId = tc.requirementId;
        tcItem->jiraTicket = tc.jiraTicket;
        tcItem->tags = tc.tags;
        tcItem->parent = componentItems[component];
        componentItems[component]->children.append(tcItem);
    }
}

//=============================================================================
// TestRepository Implementation
//=============================================================================

TestRepository& TestRepository::instance()
{
    static TestRepository instance;
    return instance;
}

TestRepository::TestRepository()
{
    m_treeModel = new TestTreeModel(this);
}

TestRepository::~TestRepository() = default;

bool TestRepository::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Clear existing data
    clear();
    
    // Load suites
    QJsonArray suitesArray = root["suites"].toArray();
    for (const auto& suiteValue : suitesArray) {
        TestSuite suite = TestSuite::fromJson(suiteValue.toObject());
        m_testSuites[suite.id] = suite;
    }
    
    // Load test cases
    QJsonArray testCasesArray = root["testCases"].toArray();
    for (const auto& tcValue : testCasesArray) {
        TestCase tc = TestCase::fromJson(tcValue.toObject());
        m_testCases[tc.id] = tc;
    }
    
    m_currentFilePath = filePath;
    setDirty(false);
    
    m_treeModel->refresh();
    
    emit repositoryLoaded(filePath);
    return true;
}

bool TestRepository::saveToFile(const QString& filePath) const
{
    QJsonObject root;
    
    // Save suites
    QJsonArray suitesArray;
    for (const auto& suite : m_testSuites) {
        suitesArray.append(suite.toJson());
    }
    root["suites"] = suitesArray;
    
    // Save test cases
    QJsonArray testCasesArray;
    for (const auto& tc : m_testCases) {
        testCasesArray.append(tc.toJson());
    }
    root["testCases"] = testCasesArray;
    
    // Metadata
    QJsonObject meta;
    meta["version"] = "1.0";
    meta["exportDate"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    meta["testCount"] = m_testCases.size();
    meta["suiteCount"] = m_testSuites.size();
    root["metadata"] = meta;
    
    QJsonDocument doc(root);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    const_cast<TestRepository*>(this)->m_currentFilePath = filePath;
    const_cast<TestRepository*>(this)->setDirty(false);
    
    emit const_cast<TestRepository*>(this)->repositorySaved(filePath);
    return true;
}

int TestRepository::importFromFile(const QString& filePath, bool overwriteExisting)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject root = doc.object();
    QJsonArray testCasesArray = root["testCases"].toArray();
    
    int imported = 0;
    for (const auto& tcValue : testCasesArray) {
        TestCase tc = TestCase::fromJson(tcValue.toObject());
        
        if (!m_testCases.contains(tc.id)) {
            addTestCase(tc);
            ++imported;
        } else if (overwriteExisting) {
            updateTestCase(tc);
            ++imported;
        }
    }
    
    return imported;
}

bool TestRepository::exportToFile(const QStringList& testIds, const QString& filePath) const
{
    QJsonArray testCasesArray;
    for (const QString& id : testIds) {
        if (m_testCases.contains(id)) {
            testCasesArray.append(m_testCases[id].toJson());
        }
    }
    
    QJsonObject root;
    root["testCases"] = testCasesArray;
    
    QJsonObject meta;
    meta["version"] = "1.0";
    meta["exportDate"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    meta["testCount"] = testCasesArray.size();
    root["metadata"] = meta;
    
    QJsonDocument doc(root);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

TestCase* TestRepository::testCase(const QString& id)
{
    if (m_testCases.contains(id)) {
        return &m_testCases[id];
    }
    return nullptr;
}

const TestCase* TestRepository::testCase(const QString& id) const
{
    auto it = m_testCases.constFind(id);
    if (it == m_testCases.constEnd()) {
        return nullptr;
    }
    return &it.value();
}

bool TestRepository::addTestCase(const TestCase& testCase)
{
    if (m_testCases.contains(testCase.id)) {
        return false;
    }
    
    m_testCases[testCase.id] = testCase;
    m_testCases[testCase.id].createdDate = QDateTime::currentDateTime();
    m_testCases[testCase.id].modifiedDate = QDateTime::currentDateTime();
    
    setDirty(true);
    m_treeModel->refresh();
    
    emit testCaseAdded(testCase.id);
    return true;
}

bool TestRepository::updateTestCase(const TestCase& testCase)
{
    if (!m_testCases.contains(testCase.id)) {
        return false;
    }
    
    m_testCases[testCase.id] = testCase;
    m_testCases[testCase.id].modifiedDate = QDateTime::currentDateTime();
    
    setDirty(true);
    m_treeModel->refresh();
    
    emit testCaseUpdated(testCase.id);
    return true;
}

bool TestRepository::removeTestCase(const QString& id)
{
    if (!m_testCases.contains(id)) {
        return false;
    }
    
    m_testCases.remove(id);
    
    setDirty(true);
    m_treeModel->refresh();
    
    emit testCaseRemoved(id);
    return true;
}

TestSuite* TestRepository::testSuite(const QString& id)
{
    if (m_testSuites.contains(id)) {
        return &m_testSuites[id];
    }
    return nullptr;
}

bool TestRepository::addTestSuite(const TestSuite& suite)
{
    if (m_testSuites.contains(suite.id)) {
        return false;
    }
    m_testSuites[suite.id] = suite;
    setDirty(true);
    m_treeModel->refresh();
    return true;
}

bool TestRepository::updateTestSuite(const TestSuite& suite)
{
    if (!m_testSuites.contains(suite.id)) {
        return false;
    }
    m_testSuites[suite.id] = suite;
    setDirty(true);
    m_treeModel->refresh();
    return true;
}

bool TestRepository::removeTestSuite(const QString& id)
{
    if (!m_testSuites.contains(id)) {
        return false;
    }
    m_testSuites.remove(id);
    setDirty(true);
    m_treeModel->refresh();
    return true;
}

QList<TestCase> TestRepository::findTestCases(const QString& searchText,
                                               const QStringList& tags,
                                               const QString& component) const
{
    QList<TestCase> results;
    
    for (const auto& tc : m_testCases) {
        // Check search text
        bool matchesText = searchText.isEmpty() ||
            tc.name.contains(searchText, Qt::CaseInsensitive) ||
            tc.description.contains(searchText, Qt::CaseInsensitive) ||
            tc.id.contains(searchText, Qt::CaseInsensitive);
        
        // Check tags
        bool matchesTags = tags.isEmpty();
        if (!matchesTags) {
            for (const QString& tag : tags) {
                if (tc.tags.contains(tag, Qt::CaseInsensitive)) {
                    matchesTags = true;
                    break;
                }
            }
        }
        
        // Check component
        bool matchesComponent = component.isEmpty() ||
            tc.component.compare(component, Qt::CaseInsensitive) == 0;
        
        if (matchesText && matchesTags && matchesComponent) {
            results.append(tc);
        }
    }
    
    return results;
}

QList<TestCase> TestRepository::testCasesByTag(const QString& tag) const
{
    QList<TestCase> results;
    for (const auto& tc : m_testCases) {
        if (tc.tags.contains(tag, Qt::CaseInsensitive)) {
            results.append(tc);
        }
    }
    return results;
}

QList<TestCase> TestRepository::testCasesByComponent(const QString& component) const
{
    QList<TestCase> results;
    for (const auto& tc : m_testCases) {
        if (tc.component.compare(component, Qt::CaseInsensitive) == 0) {
            results.append(tc);
        }
    }
    return results;
}

QStringList TestRepository::allTags() const
{
    QSet<QString> tags;
    for (const auto& tc : m_testCases) {
        for (const QString& tag : tc.tags) {
            tags.insert(tag);
        }
    }
    return tags.values();
}

QStringList TestRepository::allComponents() const
{
    QSet<QString> components;
    for (const auto& tc : m_testCases) {
        if (!tc.component.isEmpty()) {
            components.insert(tc.component);
        }
    }
    return components.values();
}

TestCase TestRepository::createNewTestCase()
{
    TestCase tc;
    tc.id = TestCase::generateId("TC");
    tc.name = "New Test Case";
    tc.description = "Enter test description";
    tc.requirementId = "REQ-XXX";
    tc.jiraTicket = "PROJ-XXX";
    tc.priority = 5;
    tc.enabled = true;
    tc.timeoutMs = 60000;
    tc.createdDate = QDateTime::currentDateTime();
    tc.modifiedDate = QDateTime::currentDateTime();
    return tc;
}

void TestRepository::clear()
{
    m_testCases.clear();
    m_testSuites.clear();
    m_currentFilePath.clear();
    setDirty(false);
    m_treeModel->refresh();
}

void TestRepository::setDirty(bool dirty)
{
    if (m_dirty != dirty) {
        m_dirty = dirty;
        emit dirtyStateChanged(dirty);
    }
}

} // namespace TestExecutor
