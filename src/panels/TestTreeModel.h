#ifndef SAMPLETESTTREEMODEL_H
#define SAMPLETESTTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

class SampleTestTreeItem
{
public:
    explicit SampleTestTreeItem(const QVector<QVariant> &data, SampleTestTreeItem *parent = nullptr);
    ~SampleTestTreeItem();

    void appendChild(SampleTestTreeItem *child);

    SampleTestTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    SampleTestTreeItem *parentItem();

private:
    QVector<SampleTestTreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    SampleTestTreeItem *m_parentItem;
};

class SampleTestTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SampleTestTreeModel(const QString &data, QObject *parent = nullptr);
    ~SampleTestTreeModel();

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
    void setupModelData(const QStringList &lines, SampleTestTreeItem *parent);
    SampleTestTreeItem *rootItem;
};

#endif // SAMPLETESTTREEMODEL_H
