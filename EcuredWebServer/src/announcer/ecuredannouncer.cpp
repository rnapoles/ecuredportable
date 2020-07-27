#include "ecuredannouncer.h"
#include <QUdpSocket>
#include "serializer.h"
#include "qobjecthelper.h"
#include "static.h"
#include <QCoreApplication>
#include <QApplication>
#include "clucenehelper.h"

EcuredAnnouncer::EcuredAnnouncer(QObject *parent) :
    QThread(parent)
{
    DatabaseManager dbm("wiki.db");

    if(dbm.OpenDB()){
        dbm.GetDbInfo(dbinfo);
        dbm.CloseDb();
    }

    QApplication* app = qApp;
    QString configFileName=Static::getConfigDir()+"/"+QCoreApplication::applicationName()+".ini";
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    listenerSettings->beginGroup("listener");
    int port=listenerSettings->value("port").toInt();
    dbinfo.setWebServerPort(port);
}

void EcuredAnnouncer::run()
{

    QUdpSocket udpSocket;

    QJson::Serializer serializer;

    CLuceneHelper *lucene = CLuceneHelper::getInstance();
    QStringList FieldNames = lucene->getFieldNames();
    dbinfo.setFieldNames(FieldNames);

    QVariantMap json = QJson::QObjectHelper::qobject2qvariant(&dbinfo);
    QByteArray datagram = serializer.serialize(json);

    while (this->isRunning()){
        QThread::sleep(5);
        //qDebug()<< "write data";
        //qDebug()<<datagram;
        udpSocket.writeDatagram(datagram.data(), datagram.size(),QHostAddress::Broadcast, 45454);
    }
}
