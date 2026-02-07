#include "TestTreeModel.h"

// ---------------------------------------------------------------------------
// SampleTestTreeItem Implementation
// ---------------------------------------------------------------------------

SampleTestTreeItem::SampleTestTreeItem(const QVector<QVariant> &data, SampleTestTreeItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}

SampleTestTreeItem::~SampleTestTreeItem()
{
    qDeleteAll(m_childItems);
}

void SampleTestTreeItem::appendChild(SampleTestTreeItem *item)
{
    m_childItems.append(item);
}

SampleTestTreeItem *SampleTestTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int SampleTestTreeItem::childCount() const
{
    return m_childItems.count();
}

int SampleTestTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant SampleTestTreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

int SampleTestTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<SampleTestTreeItem*>(this));
    return 0;
}

SampleTestTreeItem *SampleTestTreeItem::parentItem()
{
    return m_parentItem;
}

// ---------------------------------------------------------------------------
// SampleTestTreeModel Implementation
// ---------------------------------------------------------------------------

SampleTestTreeModel::SampleTestTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new SampleTestTreeItem({tr("Test Sequence")});
    setupModelData(data.split('\n'), rootItem);
}

SampleTestTreeModel::~SampleTestTreeModel()
{
    delete rootItem;
}

int SampleTestTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<SampleTestTreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant SampleTestTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    SampleTestTreeItem *item = static_cast<SampleTestTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags SampleTestTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant SampleTestTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex SampleTestTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SampleTestTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SampleTestTreeItem*>(parent.internalPointer());

    SampleTestTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SampleTestTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SampleTestTreeItem *childItem = static_cast<SampleTestTreeItem*>(index.internalPointer());
    SampleTestTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SampleTestTreeModel::rowCount(const QModelIndex &parent) const
{
    SampleTestTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SampleTestTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void SampleTestTreeModel::setupModelData(const QStringList &lines, SampleTestTreeItem *parent)
{
    Q_UNUSED(lines);
    // Hardcoded structure generation based on user requirements for now
    // We ignore the input string for this specific layout to match the image exactly
    // In a real app, we would parse the 'lines' or load from file.
    
    // --------------------------------------------------------------------------
    // Test Suite: ManDiag_SWE5 (Added as per request)
    // --------------------------------------------------------------------------
    SampleTestTreeItem *suite5 = new SampleTestTreeItem({"ManDiag_SWE5"}, parent);
    parent->appendChild(suite5);

    // Group 1: 0x0001_Enter_ManDag_Session
    SampleTestTreeItem *group5_1 = new SampleTestTreeItem({"0x0001_Enter_ManDag_Session"}, suite5);
    suite5->appendChild(group5_1);

        // TestCase 1: Enter MD Session
        SampleTestTreeItem *tc5_1 = new SampleTestTreeItem({"TestCase_01_Enter_MD_Session"}, group5_1);
        group5_1->appendChild(tc5_1);
            tc5_1->appendChild(new SampleTestTreeItem({"Test Description"}, tc5_1));
            tc5_1->appendChild(new SampleTestTreeItem({"Turn ON PPS"}, tc5_1));
            tc5_1->appendChild(new SampleTestTreeItem({"SEND SET VOLTAGE"}, tc5_1));
            tc5_1->appendChild(new SampleTestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc5_1));
            tc5_1->appendChild(new SampleTestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc5_1));

        // TestCase 2: Exit MD Session
        SampleTestTreeItem *tc5_2 = new SampleTestTreeItem({"TestCase_01_Exit_MD_Session"}, group5_1);
        group5_1->appendChild(tc5_2);
            tc5_2->appendChild(new SampleTestTreeItem({"Test Description"}, tc5_2));
            tc5_2->appendChild(new SampleTestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc5_2));
            tc5_2->appendChild(new SampleTestTreeItem({"SEND SET VOLTAGE"}, tc5_2));
            tc5_2->appendChild(new SampleTestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc5_2));

    // Group 2: 0x0002_Power_Mode
    SampleTestTreeItem *group5_2 = new SampleTestTreeItem({"0x0002_Power_Mode"}, suite5);
    suite5->appendChild(group5_2);

        // TestCase 3: Enter Sleep
        SampleTestTreeItem *tc5_3 = new SampleTestTreeItem({"TestCase_01_Enter_Sleep"}, group5_2);
        group5_2->appendChild(tc5_3);
            tc5_3->appendChild(new SampleTestTreeItem({"Test Description"}, tc5_3));
            tc5_3->appendChild(new SampleTestTreeItem({"TURN OFF CL15"}, tc5_3));
            tc5_3->appendChild(new SampleTestTreeItem({"WAIT 15 Sec"}, tc5_3));
            tc5_3->appendChild(new SampleTestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc5_3));
            tc5_3->appendChild(new SampleTestTreeItem({"READ CURRENT"}, tc5_3));

        // TestCase 4: Enter S2R
        SampleTestTreeItem *tc5_4 = new SampleTestTreeItem({"TestCase_02_Enter_S2R"}, group5_2);
        group5_2->appendChild(tc5_4);
            tc5_4->appendChild(new SampleTestTreeItem({"Test Description"}, tc5_4));
            tc5_4->appendChild(new SampleTestTreeItem({"TURN OFF CL15"}, tc5_4));
            tc5_4->appendChild(new SampleTestTreeItem({"WAIT 15 Sec"}, tc5_4));
            tc5_4->appendChild(new SampleTestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc5_4));
            tc5_4->appendChild(new SampleTestTreeItem({"READ CURRENT"}, tc5_4));

    // --------------------------------------------------------------------------
    // Test Suite: ManDiag_SWE6
    // --------------------------------------------------------------------------
    SampleTestTreeItem *suite = new SampleTestTreeItem({"ManDiag_SWE6"}, parent);
    parent->appendChild(suite);

    // Level 2: Group 1
    SampleTestTreeItem *group1 = new SampleTestTreeItem({"0x0001_Enter_ManDag_Session"}, suite);
    suite->appendChild(group1);

        // Level 3: TestCase 1
        SampleTestTreeItem *tc1 = new SampleTestTreeItem({"TestCase_01_Enter_MD_Session"}, group1);
        group1->appendChild(tc1);
            tc1->appendChild(new SampleTestTreeItem({"Test Description"}, tc1));
            tc1->appendChild(new SampleTestTreeItem({"Turn ON PPS"}, tc1));
            tc1->appendChild(new SampleTestTreeItem({"SEND SET VOLTAGE"}, tc1));
            tc1->appendChild(new SampleTestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc1));
            tc1->appendChild(new SampleTestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc1));

        // Level 3: TestCase 2
        SampleTestTreeItem *tc2 = new SampleTestTreeItem({"TestCase_01_Exit_MD_Session"}, group1);
        group1->appendChild(tc2);
            tc2->appendChild(new SampleTestTreeItem({"Test Description"}, tc2));
            tc2->appendChild(new SampleTestTreeItem({"SEND MD_REQ_GET_TYPE1"}, tc2));
            tc2->appendChild(new SampleTestTreeItem({"SEND SET VOLTAGE"}, tc2));
            tc2->appendChild(new SampleTestTreeItem({"SEND MD_REQ_SET_TYPE1"}, tc2));

    // Level 2: Group 2
    SampleTestTreeItem *group2 = new SampleTestTreeItem({"0x0002_Power_Mode"}, suite);
    suite->appendChild(group2);

        // Level 3: TestCase 3
        SampleTestTreeItem *tc3 = new SampleTestTreeItem({"TestCase_01_Enter_Sleep"}, group2);
        group2->appendChild(tc3);
            tc3->appendChild(new SampleTestTreeItem({"Test Description"}, tc3));
            tc3->appendChild(new SampleTestTreeItem({"TURN OFF CL15"}, tc3));
            tc3->appendChild(new SampleTestTreeItem({"WAIT 15 Sec"}, tc3));
            tc3->appendChild(new SampleTestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc3));
            tc3->appendChild(new SampleTestTreeItem({"READ CURRENT"}, tc3));

        // Level 3: TestCase 4
        SampleTestTreeItem *tc4 = new SampleTestTreeItem({"TestCase_02_Enter_S2R"}, group2);
        group2->appendChild(tc4);
            tc4->appendChild(new SampleTestTreeItem({"Test Description"}, tc4));
            tc4->appendChild(new SampleTestTreeItem({"TURN OFF CL15"}, tc4));
            tc4->appendChild(new SampleTestTreeItem({"WAIT 15 Sec"}, tc4));
            tc4->appendChild(new SampleTestTreeItem({"CHECK SERIAL FOR SLEEP"}, tc4));
            tc4->appendChild(new SampleTestTreeItem({"READ CURRENT"}, tc4));
}
