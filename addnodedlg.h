#ifndef ADDNODEDLG_H
#define ADDNODEDLG_H

#include <QDialog>

namespace Ui {
    class AddNodeDlg;
}

class AddNodeDlg : public QDialog {
    Q_OBJECT
public:
    AddNodeDlg(QWidget *parent = 0);
    ~AddNodeDlg();
    QString getValue();
    QString getType();
    QString getName();

protected:
    void changeEvent(QEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Ui::AddNodeDlg *ui;
    QString m_value, m_type, m_name;

private slots:
    void okButton();
    void onCurrentIndexChanged(QString currentText);
};

#endif // ADDNODEDLG_H
