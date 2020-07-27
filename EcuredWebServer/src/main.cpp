#include "../lib/qtsingleapplication/QtSingleApplication"
#include "servicemanager.h"
#include "announcer/ecuredannouncer.h"
#include <QLocale>
#include <QTextCodec>

int main(int argc, char *argv[]) {

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

    app.setApplicationVersion("2.0");


    Servicemanager Service;
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),&Service, SLOT(handleMessage(const QString&)));

    EcuredAnnouncer Announcer;
    Announcer.start();


    return app.exec();
}
