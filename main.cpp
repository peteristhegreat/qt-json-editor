#include <QApplication>
#include <QTextCodec>
#include "jsoneditormain.h"
#include <QSettings>
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));

    QSettings::setDefaultFormat(QSettings::IniFormat);
    a.setApplicationName(VER_INTERNALNAME_STR);
    a.setOrganizationName(VER_COMPANYNAME_STR);
    JsonEditorMain w;
    w.show();
    return a.exec();
}
