#include <QApplication>
#include <QTextCodec>
#include "jsoneditormain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));
    JsonEditorMain w;
    w.show();
    return a.exec();
}
