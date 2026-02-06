#ifndef TESTTREEMODEL_H
#define TESTTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

class TestTreeItem
{
public:
    explicit TestTreeItem(const QVector<QVariant> &data, TestTreeItem *parent = nullptr);
    ~TestTreeItem();

    void appendChild(TestTreeItem *child);

    TestTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TestTreeItem *parentItem();

private:
    QVector<TestTreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TestTreeItem *m_parentItem;
};

class TestTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TestTreeModel(const QString &data, QObject *parent = nullptr);
    ~TestTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList &lines, TestTreeItem *parent);
    TestTreeItem *rootItem;
};

#endif // TESTTREEMODEL_H
