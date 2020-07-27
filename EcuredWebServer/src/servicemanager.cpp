#include "servicemanager.h"
#include "static.h"
#include "dualfilelogger.h"
#include "requestmapper.h"
#include "controller/staticfilecontroller.h"
#include <QDir>
#include <QApplication>
/** Name of this application */
#define APPNAME "EcuredWebServer"

/** Publisher of this application */
#define ORGANISATION "UCI"

/** Short description of this application */
#define DESCRIPTION "Ecured Web Server"

Servicemanager::Servicemanager(QObject *parent) :
    QObject(parent)
{

    QApplication* app = qApp;
    app->setApplicationName(APPNAME);
    app->setOrganizationName(ORGANISATION);
    QString configFileName=Static::getConfigDir()+"/"+QCoreApplication::applicationName()+".ini";

    // Configure logging into files
    QSettings* mainLogSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    mainLogSettings->beginGroup("mainLogFile");
    QSettings* debugLogSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    debugLogSettings->beginGroup("debugLogFile");
    Logger* logger=new DualFileLogger(mainLogSettings,debugLogSettings,10000,app);
    logger->installMsgHandler();

    // Configure template loader and cache
    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    templateSettings->beginGroup("templates");
    Static::templateLoader=new TemplateCache(templateSettings,app);

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    sessionSettings->beginGroup("sessions");
    Static::sessionStore=new HttpSessionStore(sessionSettings,app);

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    fileSettings->beginGroup("docroot");
    Static::staticFileController=new StaticFileController(fileSettings,app);

    // Configure and start the TCP listener
    qDebug("ServiceHelper: Starting service");
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    listenerSettings->beginGroup("listener");
    this->httplistener = new HttpListener(listenerSettings,new RequestMapper(app),app);

    qDebug("ServiceHelper: Service has started");

    trayIcon = new QSystemTrayIcon(QIcon(":/Logotipo.ico"),0);
    trayIcon->show();
    quitAction = new QAction(QIcon(":/images/exit.png"),"&Salir", this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu = new QMenu();
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
}

Servicemanager::~Servicemanager()
{
    trayIcon->hide();
}


void Servicemanager::StartApp()
{

}

void Servicemanager::StopApp()
{

}

void Servicemanager::handleMessage(const QString &message)
{
    trayIcon->showMessage("EcuRed Portable","La aplicación ya está corriendo.",QSystemTrayIcon::Information,5000);
}
