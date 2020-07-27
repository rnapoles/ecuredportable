#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QLocale>
#include <QTextCodec>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char *argv[])
{

    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);

    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Cuba));




    QApplication a(argc, argv);

    //Locales para el idioma
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    WikiDbAnalizerMainWindow w;
    w.show();
    
    return a.exec();
}
