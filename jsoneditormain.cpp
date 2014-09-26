#include "jsoneditormain.h"
#include "ui_jsoneditormain.h"
#include "jsontreemodel.h"
#include "addnodedlg.h"
#include "jsedit.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
//#include <QtGui>

JsonEditorMain::JsonEditorMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JsonEditorMain),
    newInsertText(tr("Insert new data")),
    treeViewColumnKey(tr("Node")),
    treeViewColumnValue(tr("Value")),
    treeViewColumnType(tr("Type"))
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


//    connect(ui->menuNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->menuOpenFile, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->menuSaveFile, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->menuSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    JSHighlighter * highlight = new JSHighlighter(ui->jsonCode->document());

    setCurrentFile("");

    textEdit = ui->jsonCode;
    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    setUnifiedTitleAndToolBarOnMac(true);

    m_findDialog = new FindDialog(this);
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(textEdit);

    m_findReplaceDialog = new FindReplaceDialog(this);
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(textEdit);

    connect(ui->menuFind, SIGNAL(triggered()), m_findDialog, SLOT(show()));
    connect(ui->menuReplace, SIGNAL(triggered()), m_findReplaceDialog, SLOT(show()));

//    connect(ui->actionFindNext, SIGNAL(triggered()), m_findDialog, SLOT(findNext()));
//    connect(ui->actionFindPrevious, SIGNAL(triggered()), m_findDialog, SLOT(findPrev()));

    readSettings();
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
        if(ui->jsonTree->selectionModel() == 0)
        {
            QMessageBox::warning(this,this->windowTitle(),"Adding a node to an empty tree is not supported.");
            return;
        }

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

    if (objectType.compare(tr("object"), Qt::CaseInsensitive) == 0
        || objectType.compare(treeViewColumnType, Qt::CaseInsensitive) == 0)
    {
        resultStr += "{\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ");
        }
        if(treeItem->childCount() > 0)
            resultStr.remove(resultStr.length() - 2, 1);// remove trailing comma
        resultStr += indent + "},\n";
    }
    else if (objectType.compare(tr("array"), Qt::CaseInsensitive) == 0)
    {
        resultStr += "[\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ", true);
        }
        if(treeItem->childCount() > 0)
            resultStr.remove(resultStr.length() - 2, 1);// remove trailing comma
        resultStr += indent + "],\n";
    }
    else
    {
        if (objectType.compare(tr("string"), Qt::CaseInsensitive) == 0 ||
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
    QMessageBox::information(this, tr("Using Help"), tr(
		"1 On the left side of the main interface to fill the JSON code \n"
		"2 Click on the menu \" Generate code tree \", the right of the main interface will spanning tree \n"
		"3 To insert or delete after the tree node or modify the value, click on the menu \" Generate Code \", will generate JSON code from the tree."												  ));
}

void JsonEditorMain::dataEdit(QModelIndex editIndex)
{
    QString vType = editIndex.sibling(editIndex.row(), 2).data(Qt::EditRole).toString();

    if (vType.compare(tr("object")) != 0
        && vType.compare(tr("array")) != 0)
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

void JsonEditorMain::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void JsonEditorMain::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void JsonEditorMain::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"Open Json File",lastFilePath);
        if (!fileName.isEmpty())
        {
            QFileInfo info(fileName);
            lastFilePath = info.absoluteDir().absolutePath();
            loadFile(fileName);
        }
    }

    ui->menuRefresh->trigger();
}

bool JsonEditorMain::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool JsonEditorMain::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty())
        return false;

    return saveFile(files.at(0));
}

void JsonEditorMain::readSettings()
{
    QSettings settings;
//    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
//    QSize size = settings.value("size", QSize(400, 400)).toSize();
//    resize(size);
//    move(pos);
    this->restoreGeometry(settings.value("geometry").toByteArray());
    lastFilePath = (settings.value("file_path").toString());
    curFile = settings.value("last_opened_file").toString();
    if(this->curFile.length() != 0)
    {
        loadFile(curFile);
        ui->menuRefresh->trigger();
    }
}

void JsonEditorMain::writeSettings()
{
    QSettings settings;
//    settings.setValue("pos", pos());
//    settings.setValue("size", size());
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("file_path", lastFilePath);
    settings.setValue("last_opened_file", this->curFile);
}

bool JsonEditorMain::maybeSave()
{
    if (ui->jsonCode->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, this->windowTitle(),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void JsonEditorMain::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool JsonEditorMain::saveFile(const QString &fileName)
{
    ui->menuFormat->trigger();

    QFile file(fileName);
    QFileInfo info(fileName);
    lastFilePath = info.absoluteDir().absolutePath();
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void JsonEditorMain::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->jsonCode->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.json";
    setWindowFilePath(shownName);
}

QString JsonEditorMain::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
