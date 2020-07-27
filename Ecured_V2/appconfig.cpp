#include "appconfig.h"

AppConfig *AppConfig::s_instance = 0;

AppConfig::AppConfig(QObject *parent):QObject(parent)
{
    this->p_DirIndex = "";
    //this->WikiDb = "wikidb";
    this->p_WikiDb = "";
    this->p_IsSourceDocAvailable = false;
    this->p_IsUsingRemoteServer = false;
    this->p_ServerPort=0;
    this->p_DatabaseInfo = new DatabaseInfo();
}

void AppConfig::setWikiDb(QString p_WikiDb) {
    this->p_WikiDb = p_WikiDb;
    //emit configchange();
}

QString AppConfig::getWikiDb() {
    return this->p_WikiDb;
}

void AppConfig::setDirIndex(QString p_DirIndex) {
    this->p_DirIndex = p_DirIndex;
    //emit configchange();
}

QString AppConfig::getDirIndex() {
    return this->p_DirIndex;
}

void AppConfig::setServerInfo(QString p_ServerInfo) {
    //emit configchange();
    this->p_ServerInfo = p_ServerInfo;
}

QString AppConfig::getServerInfo() {
    return this->p_ServerInfo;
}

void AppConfig::setServerIP(QString p_ServerIP) {
    //emit configchange();
    this->p_ServerIP = p_ServerIP;
}

QString AppConfig::getServerIP() {
    return this->p_ServerIP;
}

void AppConfig::setServerURL(QString p_ServerURL) {
    this->p_ServerURL = p_ServerURL;
    //emit configchange();
}

QString AppConfig::getServerURL() {
    if(this->p_IsUsingRemoteServer){
        this->p_ServerURL = QString("http://%1:%2/").arg(this->p_ServerIP).arg(this->p_ServerPort);
    } else {
        this->p_ServerURL="http://ecured/";
    }
    return this->p_ServerURL;
}

void AppConfig::setSourceDocAvailable(bool p_IsSourceDocAvailable) {
    this->p_IsSourceDocAvailable = p_IsSourceDocAvailable;
    emit configchange();
}

bool AppConfig::IsSourceDocAvailable() {
    return this->p_IsSourceDocAvailable;
}

void AppConfig::setUsingRemoteServer(bool p_IsUsingRemoteServer) {
    this->p_IsUsingRemoteServer = p_IsUsingRemoteServer;
    //emit configchange();
}

bool AppConfig::IsUsingRemoteServer() {
    return this->p_IsUsingRemoteServer;
}

void AppConfig::setServerPort(int p_ServerPort) {
    this->p_ServerPort = p_ServerPort;
    //emit configchange();
}

int AppConfig::getServerPort() {
    return this->p_ServerPort;
}

DatabaseInfo *AppConfig::getDatabaseInfo() const
{
    return this->p_DatabaseInfo;
}


