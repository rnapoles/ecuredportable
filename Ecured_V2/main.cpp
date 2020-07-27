//#include <QtGui/QApplication>
#include "libs/qtsingleapplication/qtsingleapplication.h"
#include <QSplashScreen>
#include <QTimer>
#include <QBitmap>
#include <QMessageBox>
#include <QPlastiqueStyle>
#include <QPainter>
#include "destructor.h"
#include <databasemanager.h>
#include <exception>
#include <stdio.h>
#include <QLocale>
#include "ecuredsplashscreen.h"
//#include <singleton.h>

#include <BreakpadHandler.h>
#include <ErrorReporter/reporter.h>

#include <QDesktopServices>
#include "mainwindow.h"
#include "appconfig.h"

int main(int argc, char *argv[])
{
    //qDebug()<< __FUNCTION__ ;
    //qDebug()<< __PRETTY_FUNCTION__ ;

    QtSingleApplication app(argc, argv);

    if (app.isRunning()) {
        QString msg("show");
        app.sendMessage(msg, 5000);
        return 0;
    }

    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);
    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Cuba));
    //QApplication app(argc, argv);

    app.setOrganizationName("UCI");
    app.setApplicationName("Ecured Portable");

    //Manejador de Errores
    QString path = QDir::toNativeSeparators(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"\\crashes");
    //qDebug()<<path;
    BreakpadQt::GlobalHandler *ErrorHandler = BreakpadQt::GlobalHandler::instance();
    ErrorHandler->setDumpPath(path);
    path = QApplication::applicationDirPath()+"\\ErrorReporter";
    ErrorHandler->setReporter(path);

//    qDebug()<<QDesktopServices::storageLocation(QDesktopServices::DataLocation);
//    qDebug()<<QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
//    qDebug()<<QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
//    qDebug()<<QDesktopServices::storageLocation(QDesktopServices::CacheLocation);

     AppConfig::instance();


    //Reporter reporter;
    //reporter.setPath(path);
    //QObject::connect(ErrorHandler,SIGNAL(Error(QString,QString)),&reporter,SLOT(Error(QString,QString)),Qt::DirectConnection);


    //Locales para el idioma
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);
    Destructor des;

    app.setWindowIcon(QIcon(":/images/Logotipo.png"));
    app.connect(&app,SIGNAL(aboutToQuit()),&des,SLOT(destructor()));

    app.setApplicationVersion("2.0");
    //app.setStyleSheet("QToolTip { color:black} ;");
    app.setStyleSheet("QToolTip { background: #dddddd; color:#1C94C4;border: 1px solid white;} ;");
    QApplication::setStyle(new QPlastiqueStyle);

    QPixmap pixmap(":/images/ecuredlogo.png");


    EcuredSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();

    QString msg = "Versión %1";
    msg = msg.arg(QCoreApplication::applicationVersion());


    splash.showMessage(msg,Qt::AlignBottom|Qt::AlignRight,Qt::black);

    EcuredMainWindow *w = EcuredMainWindow::instance();
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),w, SLOT(handleMessage(const QString&)));
    app.installEventFilter(w);

    QObject::connect(&splash,SIGNAL(opaque()),w,SLOT(showMaximized()));



    return app.exec();
}
