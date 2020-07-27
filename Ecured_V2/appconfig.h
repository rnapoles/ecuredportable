#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include "databasemanager.h"

class AppConfig : public QObject
{
    Q_OBJECT
public:

    static AppConfig *instance()
    {
        if (!s_instance)
            s_instance = new AppConfig;
        return s_instance;
    }

    void setWikiDb(QString p_WikiDb);
    void setDirIndex(QString p_DirIndex);
    void setServerInfo(QString p_ServerInfo);
    void setServerIP(QString p_ServerIP);
    void setServerURL(QString p_ServerURL);
    void setSourceDocAvailable(bool p_IsSourceDocAvailable);
    void setUsingRemoteServer(bool p_IsUsingRemoteServer);
    void setServerPort(int p_ServerPort);


    //void setDatabaseInfo(DatabaseInfo databaseInfo) { this->databaseInfo = databaseInfo; }
    DatabaseInfo *getDatabaseInfo()const;

	
signals:
    void configchange();
public slots:


    QString getWikiDb();
    QString getDirIndex();
    QString getServerInfo();
    QString getServerIP();
    QString getServerURL();

    bool IsSourceDocAvailable();
    bool IsUsingRemoteServer();


    int getServerPort();

private:

    explicit AppConfig(QObject *parent = 0);

    static AppConfig *s_instance;

    //si la fuente documental esta disponible

    DatabaseInfo *p_DatabaseInfo;	
	
    QString p_WikiDb;
    QString p_DirIndex;
    QString p_ServerInfo;
    QString p_ServerIP;
    QString p_ServerURL;
	
    bool p_IsSourceDocAvailable;
    bool p_IsUsingRemoteServer;
    int p_ServerPort;
};

#endif // APPCONFIG_H
