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
#include <functional>

namespace TestExecutor {

namespace {

const QString kDefaultGroupName = QStringLiteral("Ungrouped");
const QString kDefaultFeatureName = QStringLiteral("General");

QString normalizedGroupNameInternal(const QString& value)
{
    const QString trimmed = value.trimmed();
    return trimmed.isEmpty() ? kDefaultGroupName : trimmed;
}

QString normalizedFeatureNameInternal(const QString& value)
{
    const QString trimmed = value.trimmed();
    return trimmed.isEmpty() ? kDefaultFeatureName : trimmed;
}

} // namespace

//=============================================================================
// TreeItem - Internal tree structure
//=============================================================================

struct TestTreeModel::TreeItem
{
    QString id;
    QString name;
    TreeItemType type = TreeItemType::Root;
    TreeItem* parent = nullptr;
    QVector<TreeItem*> children;
    
    TestStatus status = TestStatus::NotRun;
    Qt::CheckState checkState = Qt::Unchecked;
    
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
    return 1;
}

QVariant TestTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    
    switch (role) {
        case Qt::DisplayRole:
            if (index.column() == 0) {
                return item->name;
            }
            break;
            
        case IdRole:
            return item->id;
            
        case TypeRole:
            return static_cast<int>(item->type);
            
        case StatusRole:
            return static_cast<int>(item->status);

        case Qt::CheckStateRole:
            if (index.column() == 0 && item->type != TreeItemType::Root) {
                return item->checkState;
            }
            break;
            
        case Qt::DecorationRole:
            if (index.column() == 0) {
                if (item->type == TreeItemType::Group) {
                    // Reserved for future custom icon
                } else if (item->type == TreeItemType::Feature) {
                    // Reserved for future custom icon
                } else if (item->type == TreeItemType::TestCase) {
                    // Reserved for future custom icon
                }
            }
            break;
    }
    
    return QVariant();
}

bool TestTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.column() != 0 || role != Qt::CheckStateRole) {
        return false;
    }

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if (!item || item->type == TreeItemType::Root) {
        return false;
    }

    Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
    if (state != Qt::Checked && state != Qt::Unchecked) {
        state = Qt::Unchecked;
    }

    setCheckStateRecursive(item, state);
    updateParentsCheckState(item->parent);

    QVector<TreeItem*> changedItems;
    std::function<void(TreeItem*)> collectSubtree = [&](TreeItem* node) {
        if (!node || node == m_rootItem.get()) {
            return;
        }
        changedItems.append(node);
        for (TreeItem* child : node->children) {
            collectSubtree(child);
        }
    };
    collectSubtree(item);

    TreeItem* parentItem = item->parent;
    while (parentItem && parentItem != m_rootItem.get()) {
        changedItems.append(parentItem);
        parentItem = parentItem->parent;
    }

    for (TreeItem* changed : changedItems) {
        const QModelIndex changedIndex = indexForItem(changed, 0);
        if (changedIndex.isValid()) {
            emit dataChanged(changedIndex, changedIndex, {Qt::CheckStateRole});
        }
    }

    return true;
}

QVariant TestTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return "Test Name";
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
    if (index.column() == 0 && item->type != TreeItemType::Root) {
        flags |= Qt::ItemIsUserCheckable;
    }
    if (item->type == TreeItemType::TestCase) {
        flags |= Qt::ItemIsDragEnabled;
    }
    if (item->type == TreeItemType::Group || item->type == TreeItemType::Feature) {
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
            if (item->type != TreeItemType::TestCase) {
                continue;
            }
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

QString TestTreeModel::itemName(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QString();
    }
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->name;
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
    Q_UNUSED(index)
    return nullptr;
}

QStringList TestTreeModel::checkedTestCaseIds() const
{
    QStringList ids;
    collectCheckedTestCaseIds(m_rootItem.get(), ids);
    return ids;
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
    
    QMap<QString, TreeItem*> groupItems;
    QMap<QString, QMap<QString, TreeItem*>> featureItems;

    auto ensureGroupItem = [&](const QString& rawGroupName) -> TreeItem* {
        const QString groupName = normalizedGroupNameInternal(rawGroupName);
        if (!groupItems.contains(groupName)) {
            TreeItem* groupItem = new TreeItem();
            groupItem->type = TreeItemType::Group;
            groupItem->id = QString("group_%1").arg(groupName);
            groupItem->name = groupName;
            groupItem->parent = m_rootItem.get();
            m_rootItem->children.append(groupItem);
            groupItems[groupName] = groupItem;
        }
        return groupItems[groupName];
    };

    auto ensureFeatureItem = [&](const QString& rawGroupName, const QString& rawFeatureName) -> TreeItem* {
        const QString groupName = normalizedGroupNameInternal(rawGroupName);
        const QString featureName = normalizedFeatureNameInternal(rawFeatureName);
        TreeItem* groupItem = ensureGroupItem(groupName);
        if (!featureItems[groupName].contains(featureName)) {
            TreeItem* featureItem = new TreeItem();
            featureItem->type = TreeItemType::Feature;
            featureItem->id = QString("feature_%1_%2").arg(groupName, featureName);
            featureItem->name = featureName;
            featureItem->parent = groupItem;
            groupItem->children.append(featureItem);
            featureItems[groupName][featureName] = featureItem;
        }
        return featureItems[groupName][featureName];
    };

    QStringList groups = repo.allGroups();
    groups.sort(Qt::CaseInsensitive);
    for (const QString& group : groups) {
        ensureGroupItem(group);
        QStringList features = repo.allFeatures(group);
        features.sort(Qt::CaseInsensitive);
        for (const QString& feature : features) {
            ensureFeatureItem(group, feature);
        }
    }

    for (const auto& tc : repo.allTestCases()) {
        const QString groupName = normalizedGroupNameInternal(tc.component);
        const QString featureName = normalizedFeatureNameInternal(tc.feature);
        TreeItem* featureItem = ensureFeatureItem(groupName, featureName);

        TreeItem* tcItem = new TreeItem();
        tcItem->type = TreeItemType::TestCase;
        tcItem->id = tc.id;
        tcItem->name = tc.name;
        tcItem->status = tc.enabled ? TestStatus::NotRun : TestStatus::Skipped;
        tcItem->parent = featureItem;
        featureItem->children.append(tcItem);
    }
}

QModelIndex TestTreeModel::indexForItem(const TreeItem* item, int column) const
{
    if (!item || item == m_rootItem.get() || !item->parent) {
        return QModelIndex();
    }
    return createIndex(item->row(), column, const_cast<TreeItem*>(item));
}

void TestTreeModel::setCheckStateRecursive(TreeItem* item, Qt::CheckState state)
{
    if (!item) {
        return;
    }
    if (item->type != TreeItemType::Root) {
        item->checkState = state;
    }
    for (TreeItem* child : item->children) {
        setCheckStateRecursive(child, state);
    }
}

Qt::CheckState TestTreeModel::combinedCheckState(const TreeItem* item) const
{
    if (!item || item->children.isEmpty()) {
        return item ? item->checkState : Qt::Unchecked;
    }

    bool hasChecked = false;
    bool hasUnchecked = false;
    for (const TreeItem* child : item->children) {
        const Qt::CheckState state = child->checkState;
        if (state == Qt::PartiallyChecked) {
            return Qt::PartiallyChecked;
        }
        if (state == Qt::Checked) {
            hasChecked = true;
        } else {
            hasUnchecked = true;
        }
        if (hasChecked && hasUnchecked) {
            return Qt::PartiallyChecked;
        }
    }

    return hasChecked ? Qt::Checked : Qt::Unchecked;
}

void TestTreeModel::updateParentsCheckState(TreeItem* item)
{
    while (item && item != m_rootItem.get()) {
        item->checkState = combinedCheckState(item);
        item = item->parent;
    }
}

void TestTreeModel::collectCheckedTestCaseIds(const TreeItem* item, QStringList& ids) const
{
    if (!item) {
        return;
    }
    if (item->type == TreeItemType::TestCase && item->checkState == Qt::Checked) {
        ids.append(item->id);
    }
    for (const TreeItem* child : item->children) {
        collectCheckedTestCaseIds(child, ids);
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

    // Load persisted explorer grouping (optional)
    const QJsonArray groupsArray = root["explorerGroups"].toArray();
    for (const auto& groupValue : groupsArray) {
        const QJsonObject groupObject = groupValue.toObject();
        const QString groupName = normalizedGroupName(groupObject["name"].toString());
        m_groups.insert(groupName);

        const QJsonArray featuresArray = groupObject["features"].toArray();
        for (const auto& featureValue : featuresArray) {
            const QString featureName = normalizedFeatureName(featureValue.toString());
            m_featuresByGroup[groupName].insert(featureName);
        }
    }
    
    // Load suites
    const QJsonArray suitesArray = root["suites"].toArray();
    for (const auto& suiteValue : suitesArray) {
        TestSuite suite = TestSuite::fromJson(suiteValue.toObject());
        m_testSuites[suite.id] = suite;

        const QString groupName = normalizedGroupName(suite.component);
        m_groups.insert(groupName);
        m_featuresByGroup[groupName].insert(normalizedFeatureName(suite.name));
    }
    
    // Load test cases
    const QJsonArray testCasesArray = root["testCases"].toArray();
    for (const auto& tcValue : testCasesArray) {
        TestCase tc = TestCase::fromJson(tcValue.toObject());
        tc.component = normalizedGroupName(tc.component);
        tc.feature = normalizedFeatureName(tc.feature);
        m_testCases[tc.id] = tc;
        updateGroupingMetadata(tc);
    }
    
    m_currentFilePath = filePath;
    setDirty(false);
    
    m_treeModel->refresh();
    
    emit repositoryLoaded(filePath);
    return true;
}

bool TestRepository::saveToFile(const QString& filePath)
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

    // Save explorer grouping config
    QJsonArray groupsArray;
    QStringList groups = allGroups();
    groups.sort(Qt::CaseInsensitive);
    for (const QString& groupName : groups) {
        QJsonObject groupObject;
        groupObject["name"] = groupName;

        QJsonArray featuresArray;
        QStringList features = allFeatures(groupName);
        features.sort(Qt::CaseInsensitive);
        for (const QString& featureName : features) {
            featuresArray.append(featureName);
        }
        groupObject["features"] = featuresArray;
        groupsArray.append(groupObject);
    }
    root["explorerGroups"] = groupsArray;

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

    m_currentFilePath = filePath;
    setDirty(false);

    emit repositorySaved(filePath);
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

    const QJsonArray groupsArray = root["explorerGroups"].toArray();
    for (const auto& groupValue : groupsArray) {
        const QJsonObject groupObject = groupValue.toObject();
        const QString groupName = normalizedGroupName(groupObject["name"].toString());
        m_groups.insert(groupName);

        const QJsonArray featuresArray = groupObject["features"].toArray();
        for (const auto& featureValue : featuresArray) {
            m_featuresByGroup[groupName].insert(normalizedFeatureName(featureValue.toString()));
        }
    }

    const QJsonArray testCasesArray = root["testCases"].toArray();
    
    int imported = 0;
    for (const auto& tcValue : testCasesArray) {
        TestCase tc = TestCase::fromJson(tcValue.toObject());
        tc.component = normalizedGroupName(tc.component);
        tc.feature = normalizedFeatureName(tc.feature);
        
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

    TestCase normalized = testCase;
    normalized.component = normalizedGroupName(normalized.component);
    normalized.feature = normalizedFeatureName(normalized.feature);

    m_testCases[normalized.id] = normalized;
    m_testCases[normalized.id].createdDate = QDateTime::currentDateTime();
    m_testCases[normalized.id].modifiedDate = QDateTime::currentDateTime();
    updateGroupingMetadata(normalized);
    
    setDirty(true);
    m_treeModel->refresh();
    
    emit testCaseAdded(normalized.id);
    return true;
}

bool TestRepository::updateTestCase(const TestCase& testCase)
{
    if (!m_testCases.contains(testCase.id)) {
        return false;
    }
    
    TestCase normalized = testCase;
    normalized.component = normalizedGroupName(normalized.component);
    normalized.feature = normalizedFeatureName(normalized.feature);

    m_testCases[normalized.id] = normalized;
    m_testCases[normalized.id].modifiedDate = QDateTime::currentDateTime();
    updateGroupingMetadata(normalized);
    
    setDirty(true);
    m_treeModel->refresh();
    
    emit testCaseUpdated(normalized.id);
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
    m_groups.insert(normalizedGroupName(suite.component));
    m_featuresByGroup[normalizedGroupName(suite.component)].insert(normalizedFeatureName(suite.name));
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
    m_groups.insert(normalizedGroupName(suite.component));
    m_featuresByGroup[normalizedGroupName(suite.component)].insert(normalizedFeatureName(suite.name));
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
    components.unite(m_groups);
    return components.values();
}

QStringList TestRepository::allGroups() const
{
    QSet<QString> groups = m_groups;
    for (const auto& tc : m_testCases) {
        groups.insert(normalizedGroupName(tc.component));
    }
    return groups.values();
}

QStringList TestRepository::allFeatures(const QString& group) const
{
    const QString normalizedGroup = normalizedGroupName(group);
    QSet<QString> features = m_featuresByGroup.value(normalizedGroup);
    for (const auto& tc : m_testCases) {
        if (normalizedGroupName(tc.component).compare(normalizedGroup, Qt::CaseInsensitive) == 0) {
            features.insert(normalizedFeatureName(tc.feature));
        }
    }
    return features.values();
}

bool TestRepository::addGroup(const QString& groupName)
{
    const QString normalizedGroup = normalizedGroupName(groupName);
    if (m_groups.contains(normalizedGroup)) {
        return false;
    }
    m_groups.insert(normalizedGroup);
    setDirty(true);
    m_treeModel->refresh();
    return true;
}

bool TestRepository::addFeature(const QString& groupName, const QString& featureName)
{
    const QString normalizedGroup = normalizedGroupName(groupName);
    const QString normalizedFeature = normalizedFeatureName(featureName);

    m_groups.insert(normalizedGroup);
    if (m_featuresByGroup[normalizedGroup].contains(normalizedFeature)) {
        return false;
    }
    m_featuresByGroup[normalizedGroup].insert(normalizedFeature);
    setDirty(true);
    m_treeModel->refresh();
    return true;
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
    tc.component = kDefaultGroupName;
    tc.feature = kDefaultFeatureName;
    tc.timeoutMs = 60000;
    tc.createdDate = QDateTime::currentDateTime();
    tc.modifiedDate = QDateTime::currentDateTime();
    return tc;
}

void TestRepository::clear()
{
    m_testCases.clear();
    m_testSuites.clear();
    m_groups.clear();
    m_featuresByGroup.clear();
    m_currentFilePath.clear();
    setDirty(false);
    m_treeModel->refresh();
}

QString TestRepository::normalizedGroupName(const QString& value)
{
    return normalizedGroupNameInternal(value);
}

QString TestRepository::normalizedFeatureName(const QString& value)
{
    return normalizedFeatureNameInternal(value);
}

void TestRepository::updateGroupingMetadata(const TestCase& testCase)
{
    const QString groupName = normalizedGroupName(testCase.component);
    const QString featureName = normalizedFeatureName(testCase.feature);
    m_groups.insert(groupName);
    m_featuresByGroup[groupName].insert(featureName);
}

void TestRepository::setDirty(bool dirty)
{
    if (m_dirty != dirty) {
        m_dirty = dirty;
        emit dirtyStateChanged(dirty);
    }
}

} // namespace TestExecutor
