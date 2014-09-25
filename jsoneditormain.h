#ifndef JSONEDITORMAIN_H
#define JSONEDITORMAIN_H

#include <QMainWindow>
#include <QModelIndex>
#include "jsonc/jsoncxx.h"
#include "jsontreeitem.h"

namespace Ui {
    class JsonEditorMain;
}

class JsonEditorMain : public QMainWindow {
    Q_OBJECT
public:
    JsonEditorMain(QWidget *parent = 0);
    ~JsonEditorMain();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::JsonEditorMain *ui;
    Json::Value jsonValue;

    const QString newInsertText;
    const QString treeViewColumnKey;
    const QString treeViewColumnValue;
    const QString treeViewColumnType;

    QString treeFormat(JsonTreeItem* treeItem = NULL, QString indent = "", bool noHeader = false);

private slots:
    void toggleFileToolbar(bool checked);
    void toggleEditToolbar(bool checked);
    void toggleFindToolbar(bool checked);
    void toggleCodeToolbar(bool checked);
    void refreshJsonTree();
    void insertTreeNode();
    void deleteTreeNode();
    void insertTreeChild();
    void updateActions();
    void treeViewDataChanged();
    void formatCode();
    void showHelp();
    void dataEdit(QModelIndex editIndex);
};

#endif // JSONEDITORMAIN_H
