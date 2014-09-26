#include "addnodedlg.h"
#include "ui_addnodedlg.h"
#include <QMessageBox>

AddNodeDlg::AddNodeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNodeDlg)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okButton()));
    connect(ui->nodeType, SIGNAL(currentIndexChanged(QString)), this, SLOT(onCurrentIndexChanged(QString)));

    QStringList typeItems;
    typeItems.append(QString(tr("array")));
    typeItems.append(QString(tr("object")));
    typeItems.append(QString(tr("boolean")));
    typeItems.append(QString(tr("null")));
    typeItems.append(QString(tr("int")));
    typeItems.append(QString(tr("double")));
    typeItems.append(QString(tr("string")));
    ui->nodeType->addItems(typeItems);
}

AddNodeDlg::~AddNodeDlg()
{
    delete ui;
}

void AddNodeDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString AddNodeDlg::getValue()
{
    return m_value;
}

QString AddNodeDlg::getName()
{
    return m_name;
}

QString AddNodeDlg::getType()
{
    return m_type;
}

void AddNodeDlg::okButton()
{
    m_type = ui->nodeType->currentText();
    m_value = ui->nodeType->isEnabled() == true ? ui->nodeValue->text() : QString("");
    m_name = ui->nodeTitle->text();
}

void AddNodeDlg::onCurrentIndexChanged(QString currentText)
{
    if (currentText == tr("array")
        || currentText == tr("object")
        || currentText == tr("null"))
    {
        ui->nodeValue->setEnabled(false);
    }
    else
    {
        ui->nodeValue->setEnabled(true);
    }
}

void AddNodeDlg::hideEvent(QHideEvent *e)
{
    if (ui->nodeTitle->text().length() == 0)
    {
        QMessageBox::information(this, tr("Tip"), tr("Node name must be filled"));
        return;
    }

    if (ui->nodeType->currentText() == tr("boolean"))
        if (ui->nodeValue->text().toLower().compare(tr("true")) != 0
            && ui->nodeValue->text().toLower().compare(tr("false")) != 0)
        {
            QMessageBox::information(this, tr("Tip"), tr("Boolean values must be true or false"));
            return;
        }

    QDialog::hideEvent(e);
}
