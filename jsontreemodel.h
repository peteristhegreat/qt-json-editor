#ifndef JSONTREEMODEL_H
#define JSONTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "jsontreeitem.h"
#include "jsonc/jsoncxx.h"

class JsonTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    JsonTreeModel(const QStringList &headers, const Json::Value &data,
                 QObject *parent = 0);
    ~JsonTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole);

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex());
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());

    JsonTreeItem *getRootItem();

private:
    void setupModelData(const Json::Value &lines, JsonTreeItem *parent);
    QString getTypeString(int vType);

    JsonTreeItem *getItem(const QModelIndex &index) const;
    JsonTreeItem *rootItem;
};

#endif
