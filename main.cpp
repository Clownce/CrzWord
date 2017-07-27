#include "myword.h"
#include <QApplication>
#include <QTranslator>
#pragma execution_character_set("UTF-8")
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator *translator;
    translator=new QTranslator;
    translator->load(":/lang_zh.qm");//这3行注释了换语言程序会崩溃
    //a.installTranslator(translator);
    MyWord w;
    w.show();

    return a.exec();
}
