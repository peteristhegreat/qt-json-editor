#include <QStringList>

#include "jsontreeitem.h"

JsonTreeItem::JsonTreeItem(const QVector<QVariant> &data, JsonTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

JsonTreeItem::~JsonTreeItem()
{
    qDeleteAll(childItems);
}

JsonTreeItem *JsonTreeItem::child(int number)
{
    return childItems.value(number);
}

int JsonTreeItem::childCount() const
{
    return childItems.count();
}

int JsonTreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<JsonTreeItem*>(this));

    return 0;
}

int JsonTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant JsonTreeItem::data(int column) const
{
    return itemData.value(column);
}

bool JsonTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        JsonTreeItem *item = new JsonTreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool JsonTreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (JsonTreeItem *child, childItems)
        child->insertColumns(position, columns);

    return true;
}

JsonTreeItem *JsonTreeItem::parent()
{
    return parentItem;
}

bool JsonTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool JsonTreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (JsonTreeItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

bool JsonTreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

