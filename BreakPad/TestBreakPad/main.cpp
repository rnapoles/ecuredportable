#include <QtGui/QApplication>
#include "mainwindow.h"
#include <BreakpadHandler.h>
#include <QDesktopServices>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Manejador de Errores
    //QString path = QDir::toNativeSeparators(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"\\crashes");
    QString path = QApplication::applicationDirPath()+"\\crashes";
    qDebug()<<path;
    BreakpadQt::GlobalHandler *ErrorHandler = BreakpadQt::GlobalHandler::instance();
    ErrorHandler->setDumpPath(path);
    path = QApplication::applicationDirPath()+"\\ErrorReporter";
    ErrorHandler->setReporter(path);
    qDebug()<<path;

    MainWindow w;
    w.show();
    
    return a.exec();
}
