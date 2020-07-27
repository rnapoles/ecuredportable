#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QPlastiqueStyle>
#include <QTextCodec>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);
    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Cuba));


    QApplication a(argc, argv);
    QApplication::setStyle(new QPlastiqueStyle);
    MainWindow w;
    w.show();

    //Locales para el idioma
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    return a.exec();
}
