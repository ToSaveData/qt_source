#include "widget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("Widget_ko");
    a.installTranslator(&translator);

    Widget w;
    w.show();
    return a.exec();
}
