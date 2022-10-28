#include "chatclient.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("ChatClient_ko");
    a.installTranslator(&translator);

    ChatClient w;
    w.show();
    return a.exec();
}
