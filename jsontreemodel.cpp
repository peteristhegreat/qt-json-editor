#include <QtGui>
#include <list>
#include "jsontreemodel.h"

JsonTreeModel::JsonTreeModel(const QStringList &headers, const Json::Value &data,
                     QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;

    rootItem = new JsonTreeItem(rootData);
    setupModelData(data, rootItem);
}

JsonTreeModel::~JsonTreeModel()
{
    delete rootItem;
}

int JsonTreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}

QVariant JsonTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    JsonTreeItem *item = getItem(index);

    return item->data(index.column());
}

Qt::ItemFlags JsonTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

JsonTreeItem *JsonTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        JsonTreeItem *item = static_cast<JsonTreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

QVariant JsonTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex JsonTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    JsonTreeItem *parentItem = getItem(parent);

    JsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool JsonTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool JsonTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    JsonTreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex JsonTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    JsonTreeItem *childItem = getItem(index);
    JsonTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool JsonTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool JsonTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    JsonTreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int JsonTreeModel::rowCount(const QModelIndex &parent) const
{
    JsonTreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool JsonTreeModel::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
    if (role != Qt::EditRole)
        return false;

    JsonTreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool JsonTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

void JsonTreeModel::setupModelData(const Json::Value &lines, JsonTreeItem *parent)
{
    Json::Value* data = (Json::Value*)&lines;
    //QList<JsonTreeItem*> parents;
    QVector<QVariant> columnData;
    //parents << parent;

    /*
    JsonTreeItem *parent1 = parents.last();
    parent1->insertChildren(parent1->childCount(), 1, 2);
    parent1->child(parent1->childCount() - 1)->setData(0, QVariant(tr("A")));
    parent1->child(parent1->childCount() - 1)->setData(1, QVariant(tr("fdsfs")));

    JsonTreeItem *parent2 = parents.last()->child(0);
    parent2->insertChildren(parent2->childCount(), 1, 2);
    parent2->child(parent2->childCount() - 1)->setData(0, QVariant(tr("B")));
    parent2->child(parent2->childCount() - 1)->setData(1, QVariant(tr("fdsfs")));
    */

    int len = data->size();

    for (int i = 0; i < len; i++)
    {
        std::string key;
        int vType = data->type();
        QVariant keyIndex, keyData, keyType;
        if (vType == Json::arrayValue)
        {
            QString arrayIndex("");
            arrayIndex.append(QString().sprintf("%i", i));
            keyIndex = QVariant(arrayIndex);
            keyData = QVariant(tr((*data)[i].asString()));
            keyType = QVariant(getTypeString(((*data)[i]).type()));
        }
        else
        {
            key = data->getAllKeys().at(i);
            keyIndex = QVariant(tr(key.c_str()));
            keyData = QVariant(tr(((*data)[key]).asString()));
            keyType = QVariant(getTypeString(((*data)[key]).type()));
        }

        columnData.clear();
        columnData.push_back(keyIndex);
        columnData.push_back(keyData);
        columnData.push_back(keyType);

        JsonTreeItem *parent1 = parent;
        parent1->insertChildren(parent1->childCount(), 1, rootItem->columnCount());
        for (int column = 0; column < columnData.size(); ++column)
            parent1->child(parent1->childCount() - 1)->setData(column, columnData[column]);

        if (data->type() == Json::arrayValue)
        {
            if (((*data)[i]).type() == Json::arrayValue || ((*data)[i]).type() == Json::objectValue)
            {
                setupModelData((*data)[i], parent1->child(parent1->childCount() - 1));
            }
        }
        else
        {
            if (((*data)[key]).type() == Json::arrayValue || ((*data)[key]).type() == Json::objectValue)
            {
                setupModelData((*data)[key], parent1->child(parent1->childCount() - 1));
            }
        }
    }
}

JsonTreeItem *JsonTreeModel::getRootItem()
{
    return rootItem;
}

QString JsonTreeModel::getTypeString(int vType)
{
    QString typeString;
    switch (vType)
    {
    case Json::arrayValue:
        typeString = tr("数组");
        break;
    case Json::booleanValue:
        typeString = tr("布尔");
        break;
    case Json::intValue:
        typeString = tr("整数");
        break;
    case Json::nullValue:
        typeString = tr("空值");
        break;
    case Json::objectValue:
        typeString = tr("对象");
        break;
    case Json::realValue:
        typeString = tr("浮点数");
        break;
    case Json::stringValue:
        typeString = tr("字符串");
        break;
    case Json::uintValue:
        typeString = tr("无符号整数");
        break;
    defalut:
        typeString = tr("类型错误");
    }
    return typeString;
}
