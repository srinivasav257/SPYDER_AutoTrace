#include "TestTreeModel.h"

// ---------------------------------------------------------------------------
// TestTreeItem Implementation
// ---------------------------------------------------------------------------

TestTreeItem::TestTreeItem(const QVector<QVariant> &data, TestTreeItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}

TestTreeItem::~TestTreeItem()
{
    qDeleteAll(m_childItems);
}

void TestTreeItem::appendChild(TestTreeItem *item)
{
    m_childItems.append(item);
}

TestTreeItem *TestTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int TestTreeItem::childCount() const
{
    return m_childItems.count();
}

int TestTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TestTreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

int TestTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TestTreeItem*>(this));
    return 0;
}

TestTreeItem *TestTreeItem::parentItem()
{
    return m_parentItem;
}

// ---------------------------------------------------------------------------
// TestTreeModel Implementation
// ---------------------------------------------------------------------------

TestTreeModel::TestTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TestTreeItem({tr("Test Sequence")});
    setupModelData(data.split('\n'), rootItem);
}

TestTreeModel::~TestTreeModel()
{
    delete rootItem;
}

int TestTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TestTreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TestTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TestTreeItem *item = static_cast<TestTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags TestTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TestTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TestTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TestTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TestTreeItem*>(parent.internalPointer());

    TestTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TestTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TestTreeItem *childItem = static_cast<TestTreeItem*>(index.internalPointer());
    TestTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TestTreeModel::rowCount(const QModelIndex &parent) const
{
    TestTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TestTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TestTreeModel::setupModelData(const QStringList &lines, TestTreeItem *parent)
{
    Q_UNUSED(lines);
    // Hardcoded structure generation based on user requirements for now
    // We ignore the input string for this specific layout to match the image exactly
    // In a real app, we would parse the 'lines' or load from file.
    
    // --------------------------------------------------------------------------
    // Test Suite: ManDiag_SWE5 (Added as per request)
    // --------------------------------------------------------------------------
    TestTreeItem *suite5 = new TestTreeItem({"ManDiag_SWE5"}, parent);
    parent->appendChild(suite5);

    // Group 1: 0x0001_Enter_ManDag_Session
    TestTreeItem *group5_1 = new TestTreeItem({"0x0001_Enter_ManDag_Session"}, suite5);
    suite5->appendChild(group5_1);

        // TestCase 1: Enter MD Session
        TestTreeItem *tc5_1 = new TestTreeItem({"TestCase_01_Enter_MD_Session"}, group5_1);
        group5_1->appendChild(tc5_1);
            tc5_1->appendChild(new TestTreeItem({"Test Description"}, tc5_1));
            tc5_1->appendChild(new TestTreeItem({"Turn ON PPS"}, tc5_1));
            tc5_1->appendChild(new TestTreeItem({"SEND SET VOLTAGE"}, tc5_1));
            tc5_1->appendChild(new TestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc5_1));
            tc5_1->appendChild(new TestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc5_1));

        // TestCase 2: Exit MD Session
        TestTreeItem *tc5_2 = new TestTreeItem({"TestCase_01_Exit_MD_Session"}, group5_1);
        group5_1->appendChild(tc5_2);
            tc5_2->appendChild(new TestTreeItem({"Test Description"}, tc5_2));
            tc5_2->appendChild(new TestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc5_2));
            tc5_2->appendChild(new TestTreeItem({"SEND SET VOLTAGE"}, tc5_2));
            tc5_2->appendChild(new TestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc5_2));

    // Group 2: 0x0002_Power_Mode
    TestTreeItem *group5_2 = new TestTreeItem({"0x0002_Power_Mode"}, suite5);
    suite5->appendChild(group5_2);

        // TestCase 3: Enter Sleep
        TestTreeItem *tc5_3 = new TestTreeItem({"TestCase_01_Enter_Sleep"}, group5_2);
        group5_2->appendChild(tc5_3);
            tc5_3->appendChild(new TestTreeItem({"Test Description"}, tc5_3));
            tc5_3->appendChild(new TestTreeItem({"TURN OFF CL15"}, tc5_3));
            tc5_3->appendChild(new TestTreeItem({"WAIT 15 Sec"}, tc5_3));
            tc5_3->appendChild(new TestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc5_3));
            tc5_3->appendChild(new TestTreeItem({"READ CURRENT"}, tc5_3));

        // TestCase 4: Enter S2R
        TestTreeItem *tc5_4 = new TestTreeItem({"TestCase_02_Enter_S2R"}, group5_2);
        group5_2->appendChild(tc5_4);
            tc5_4->appendChild(new TestTreeItem({"Test Description"}, tc5_4));
            tc5_4->appendChild(new TestTreeItem({"TURN OFF CL15"}, tc5_4));
            tc5_4->appendChild(new TestTreeItem({"WAIT 15 Sec"}, tc5_4));
            tc5_4->appendChild(new TestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc5_4));
            tc5_4->appendChild(new TestTreeItem({"READ CURRENT"}, tc5_4));

    // --------------------------------------------------------------------------
    // Test Suite: ManDiag_SWE6
    // --------------------------------------------------------------------------
    TestTreeItem *suite = new TestTreeItem({"ManDiag_SWE6"}, parent);
    parent->appendChild(suite);

    // Level 2: Group 1
    TestTreeItem *group1 = new TestTreeItem({"0x0001_Enter_ManDag_Session"}, suite);
    suite->appendChild(group1);

        // Level 3: TestCase 1
        TestTreeItem *tc1 = new TestTreeItem({"TestCase_01_Enter_MD_Session"}, group1);
        group1->appendChild(tc1);
            tc1->appendChild(new TestTreeItem({"Test Description"}, tc1));
            tc1->appendChild(new TestTreeItem({"Turn ON PPS"}, tc1));
            tc1->appendChild(new TestTreeItem({"SEND SET VOLTAGE"}, tc1));
            tc1->appendChild(new TestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc1));
            tc1->appendChild(new TestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc1));

        // Level 3: TestCase 2
        TestTreeItem *tc2 = new TestTreeItem({"TestCase_01_Exit_MD_Session"}, group1);
        group1->appendChild(tc2);
            tc2->appendChild(new TestTreeItem({"Test Description"}, tc2));
            tc2->appendChild(new TestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc2));
            tc2->appendChild(new TestTreeItem({"SEND SET VOLTAGE"}, tc2));
            tc2->appendChild(new TestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc2));

    // Level 2: Group 2
    TestTreeItem *group2 = new TestTreeItem({"0x0002_Power_Mode"}, suite);
    suite->appendChild(group2);

        // Level 3: TestCase 3
        TestTreeItem *tc3 = new TestTreeItem({"TestCase_01_Enter_Sleep"}, group2);
        group2->appendChild(tc3);
            tc3->appendChild(new TestTreeItem({"Test Description"}, tc3));
            tc3->appendChild(new TestTreeItem({"TURN OFF CL15"}, tc3));
            tc3->appendChild(new TestTreeItem({"WAIT 15 Sec"}, tc3));
            tc3->appendChild(new TestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc3));
            tc3->appendChild(new TestTreeItem({"READ CURRENT"}, tc3));

        // Level 3: TestCase 4
        TestTreeItem *tc4 = new TestTreeItem({"TestCase_02_Enter_S2R"}, group2);
        group2->appendChild(tc4);
            tc4->appendChild(new TestTreeItem({"Test Description"}, tc4));
            tc4->appendChild(new TestTreeItem({"TURN OFF CL15"}, tc4));
            tc4->appendChild(new TestTreeItem({"WAIT 15 Sec"}, tc4));
            tc4->appendChild(new TestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc4));
            tc4->appendChild(new TestTreeItem({"READ CURRENT"}, tc4));
}
