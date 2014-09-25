#ifndef JSONTREEITEM_H
#define JSONTREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class JsonTreeItem
{
public:
    JsonTreeItem(const QVector<QVariant> &data, JsonTreeItem *parent = 0);
    ~JsonTreeItem();

    JsonTreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    JsonTreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);

private:
    QList<JsonTreeItem*> childItems;
    QVector<QVariant> itemData;
    JsonTreeItem *parentItem;
};

#endif
