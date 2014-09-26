#ifndef JSONEDITORMAIN_H
#define JSONEDITORMAIN_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTextEdit>
#include "jsonc/jsoncxx.h"
#include "jsontreeitem.h"
#include "finddialog.h"
#include "findreplacedialog.h"

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
    void closeEvent(QCloseEvent *event);

private:
    Ui::JsonEditorMain *ui;
    Json::Value jsonValue;

    const QString newInsertText;
    const QString treeViewColumnKey;
    const QString treeViewColumnValue;
    const QString treeViewColumnType;

    QString treeFormat(JsonTreeItem* treeItem = NULL, QString indent = "", bool noHeader = false);
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;
    QTextEdit *textEdit;
    QString lastFilePath;


    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;

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
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
};

#endif // JSONEDITORMAIN_H
