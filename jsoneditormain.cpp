#include "jsoneditormain.h"
#include "ui_jsoneditormain.h"
#include "jsontreemodel.h"
#include "addnodedlg.h"

#include <QtGui>

JsonEditorMain::JsonEditorMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JsonEditorMain),
    newInsertText(tr("插入新数据")),
    treeViewColumnKey(tr("节点")),
    treeViewColumnValue(tr("值")),
    treeViewColumnType(tr("类型"))
{
    ui->setupUi(this);
    connect(ui->menuCodeTools, SIGNAL(toggled(bool)), this, SLOT(toggleCodeToolbar(bool)));
    connect(ui->menuFindTools, SIGNAL(toggled(bool)), this, SLOT(toggleFindToolbar(bool)));
    connect(ui->menuFileTools, SIGNAL(toggled(bool)), this, SLOT(toggleFileToolbar(bool)));
    connect(ui->menuEditTools, SIGNAL(toggled(bool)), this, SLOT(toggleEditToolbar(bool)));
    connect(ui->menuRefresh, SIGNAL(triggered()), this, SLOT(refreshJsonTree()));
    connect(ui->menuInsertNode, SIGNAL(triggered()), this, SLOT(insertTreeNode()));
    connect(ui->menuInsertChild, SIGNAL(triggered()), this, SLOT(insertTreeChild()));
    connect(ui->menuDeleteNode, SIGNAL(triggered()), this, SLOT(deleteTreeNode()));
    connect(ui->jsonTree, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
    connect(ui->menuFormat, SIGNAL(triggered()), this, SLOT(formatCode()));
    connect(ui->menuHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(ui->jsonTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dataEdit(QModelIndex)));
}

JsonEditorMain::~JsonEditorMain()
{
    delete ui;
}

void JsonEditorMain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void JsonEditorMain::toggleCodeToolbar(bool checked)
{
    checked ? ui->codeToolBar->show() : ui->codeToolBar->hide();
}

void JsonEditorMain::toggleEditToolbar(bool checked)
{
    checked ? ui->editToolBar->show() : ui->editToolBar->hide();
}

void JsonEditorMain::toggleFileToolbar(bool checked)
{
    checked ? ui->fileToolBar->show() : ui->fileToolBar->hide();
}

void JsonEditorMain::toggleFindToolbar(bool checked)
{
    checked ? ui->findToolBar->show() : ui->findToolBar->hide();
}

void JsonEditorMain::refreshJsonTree()
{
    if (!ui->jsonCode->document()->isEmpty())
    {
        QByteArray ss = ui->jsonCode->toPlainText().toLocal8Bit();
        std::string json = ui->jsonCode->toPlainText().toStdString();
        Json::Reader jsonReader;
        jsonValue.clear();
        jsonReader.parse(ss.data(), jsonValue);

        QStringList headers;
        headers << treeViewColumnKey << treeViewColumnValue << treeViewColumnType;



        QAbstractItemModel *oldModel = ui->jsonTree->model();
        if (oldModel != NULL)
            oldModel->disconnect(SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        JsonTreeModel *model = new JsonTreeModel(headers, jsonValue);
        QItemSelectionModel *selectionModel = ui->jsonTree->selectionModel();
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(treeViewDataChanged()));
        ui->jsonTree->setModel(model);
        ui->jsonTree->reset();
        delete selectionModel;
        delete oldModel;


        ui->jsonTree->expandAll();

        for (int i = 0; i < ui->jsonTree->model()->columnCount(); i++)
            ui->jsonTree->resizeColumnToContents(i);
    }
}

void JsonEditorMain::insertTreeNode()
{
    AddNodeDlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        QModelIndex index = ui->jsonTree->selectionModel()->currentIndex();
        QAbstractItemModel *model = ui->jsonTree->model();

        if (!model->insertRow(index.row()+1, index.parent()))
            return;

        updateActions();

        /*
        for (int column = 0; column < model->columnCount(index.parent()); ++column)
        {
            QModelIndex child = model->index(index.row()+1, column, index.parent());
            model->setData(child, QVariant(newInsertText), Qt::EditRole);
        }
        */
        QModelIndex child = model->index(index.row()+1, 0, index.parent());
        model->setData(child, QVariant(dlg.getName()), Qt::EditRole);

        child = model->index(index.row()+1, 1, index.parent());
        model->setData(child, QVariant(dlg.getValue()), Qt::EditRole);

        child = model->index(index.row()+1, 2, index.parent());
        model->setData(child, QVariant(dlg.getType()), Qt::EditRole);
    }
}

void JsonEditorMain::insertTreeChild()
{
    AddNodeDlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        QModelIndex index = ui->jsonTree->selectionModel()->currentIndex();
        JsonTreeModel *model = (JsonTreeModel*)ui->jsonTree->model();

        if (index.column() != 0)
        {
            index = index.sibling(index.row(), 0);
        }

        if (model->columnCount(index) == 0)
        {
            if (!model->insertColumn(0, index))
                return;
        }

        if (!model->insertRow(0, index))
            return;

        /*
        for (int column = 0; column < model->columnCount(index); column++)
        {
            QModelIndex child = model->index(0, column, index);
            model->setData(child, QVariant(newInsertText), Qt::EditRole);
        }
        */

        QModelIndex child = model->index(0, 0, index);
        model->setData(child, QVariant(dlg.getName()), Qt::EditRole);

        child = model->index(0, 1, index);
        model->setData(child, QVariant(dlg.getValue()), Qt::EditRole);

        child = model->index(0, 2, index);
        model->setData(child, QVariant(dlg.getType()), Qt::EditRole);

        ui->jsonTree->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                        QItemSelectionModel::ClearAndSelect);
        updateActions();
    }
}

void JsonEditorMain::deleteTreeNode()
{
    QModelIndex index = ui->jsonTree->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->jsonTree->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void JsonEditorMain::updateActions()
 {
     bool hasSelection = !ui->jsonTree->selectionModel()->selection().isEmpty();
     ui->menuDeleteNode->setEnabled(hasSelection);

     bool hasCurrent = ui->jsonTree->selectionModel()->currentIndex().isValid();
     ui->menuInsertNode->setEnabled(hasCurrent);
     ui->menuInsertChild->setEnabled(hasCurrent);
 }

void JsonEditorMain::treeViewDataChanged()
{
    for (int i = 0; i < ui->jsonTree->model()->columnCount(); i++)
    {
        ui->jsonTree->resizeColumnToContents(i);
        ui->jsonTree->setColumnWidth(i, ui->jsonTree->columnWidth(i) + 20);
    }
}

void JsonEditorMain::formatCode()
{
    if (ui->jsonTree->model() != NULL)
    {
        ui->jsonCode->clear();

        QString codeText = "";
        JsonTreeModel *model;
        model = (JsonTreeModel*)ui->jsonTree->model();
        codeText += treeFormat(model->getRootItem(), "", true);

        ui->jsonCode->setPlainText(codeText);
    }
}

QString JsonEditorMain::treeFormat(JsonTreeItem *treeItem, QString indent, bool noHeader)
{
    QString resultStr;
    QString objectKey = treeItem->data(0).toString();
    QString objectValue = treeItem->data(1).toString();
    QString objectType = treeItem->data(2).toString();

    if (noHeader)
    {
        resultStr = indent;
    }
    else
    {
        resultStr = indent + QString("\"") + objectKey + "\": ";
    }

    if (objectType.compare(tr("对象"), Qt::CaseInsensitive) == 0
        || objectType.compare(treeViewColumnType, Qt::CaseInsensitive) == 0)
    {
        resultStr += "{\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ");
        }
        resultStr.remove(resultStr.length() - 2, 1);
        resultStr += indent + "},\n";
    }
    else if (objectType.compare(tr("数组"), Qt::CaseInsensitive) == 0)
    {
        resultStr += "[\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ", true);
        }
        resultStr.remove(resultStr.length() - 2, 1);
        resultStr += indent + "],\n";
    }
    else
    {
        if (objectType.compare(tr("字符串"), Qt::CaseInsensitive) == 0 ||
            objectType.compare(newInsertText, Qt::CaseInsensitive) == 0)
        {
            resultStr += "\"" + objectValue + "\",\n";
        }
        else
        {
            resultStr += objectValue + ",\n";
        }
    }

    return resultStr;
}

void JsonEditorMain::showHelp()
{
    QMessageBox::information(this, tr("使用帮助"), tr("1. 在主界面的左边填写入 JSON 代码\n"
                                                  "2. 点击菜单\"生成代码树\"，主界面右边会生成树\n"
                                                  "3. 给树增减节点或修改值之后，点击菜单\"生成代码\"，会从树来生成 JSON 代码"));
}

void JsonEditorMain::dataEdit(QModelIndex editIndex)
{
    QString vType = editIndex.sibling(editIndex.row(), 2).data(Qt::EditRole).toString();

    if (vType.compare(tr("对象")) != 0
        && vType.compare(tr("数组")) != 0)
    {
        if (editIndex.column() >= 2)
        {
            QModelIndex newIndex = editIndex.sibling(editIndex.row(), 1);
            ui->jsonTree->edit(newIndex);
        }
    }
    else
    {
        QModelIndex newIndex = editIndex.sibling(editIndex.row(), 0);
        ui->jsonTree->edit(newIndex);
    }
}






