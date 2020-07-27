#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>

class DatabaseInfo:public QObject {
    Q_OBJECT
    Q_PROPERTY(QString Fecha READ Fecha WRITE setFecha)
    Q_PROPERTY(bool WikiCode READ WikiCode WRITE setWikiCode)
    Q_PROPERTY(bool Compressed READ Compressed WRITE setCompressed)
    Q_PROPERTY(bool IsTableAvailable READ IsTableAvailable WRITE setTableAvailable)
    Q_PROPERTY(int NumArts READ NumArts WRITE setNumArts)
    Q_PROPERTY(int NumFields READ NumFields WRITE setNumFields)
    Q_PROPERTY(int WebServerPort READ WebServerPort WRITE setWebServerPort)
    Q_PROPERTY(QStringList FieldNames READ FieldNames WRITE setFieldNames)
	
public:
    explicit DatabaseInfo(){
        p_WikiCode=false;
        p_Compressed=false;
        p_IsTableAvailable=false;
        p_NumArts=0;
        p_NumFields=0;
    }

    //DatabaseInfo(const DatabaseInfo& other);

    QString Fecha() const;
    void setFecha(const QString& Fecha);

    bool WikiCode() const;
    void setWikiCode(const bool& WikiCode);

    bool Compressed() const;
    void setCompressed(const bool& Compressed);

    bool IsTableAvailable() const;
    void setTableAvailable(const bool& IsTableAvailable);

    quint64 NumArts() const;
    void setNumArts(const quint64& NumArts);

    qint32 NumFields() const;
    void setNumFields(const qint32& NumFields);

    qint32 WebServerPort() const;
    void setWebServerPort(const qint32& WebServerPort);

    QStringList FieldNames() const;
    void setFieldNames(const QStringList& FieldNames);
	
private:
    QString p_Fecha;
    bool p_WikiCode;
    bool p_Compressed;
    bool p_IsTableAvailable;
    quint64 p_NumArts;
    qint32 p_NumFields;
    qint32 p_WebServerPort;
	QStringList p_FieldNames;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(const QString &dbname,QObject *parent = 0);

    bool OpenDB();
    void CloseDb();
    bool DeleteDB(bool ReportErr=true);
    bool CreateDb();
    bool IsSqliteDb();
    bool SetWikitaxiHeader();
    bool GetDbInfo(DatabaseInfo &dbinfo);
    bool SetDbInfo(bool wikicode=false,bool compressed=false,quint64 NumArts=0);


    QSqlError LastErrorSql();

    QString LastErrorStr();

    bool Insert(QString title, QString text);
    bool Insert(QString title, QByteArray text);

    bool IsOpen();

    bool IsWikiTaxiDb();

    QStringList getTables();

    bool isDbInfoAvailable(){
        return DbInfoAvailable;
    }

private:
    QSqlDatabase db;
    bool wikitaxi;
    bool DbInfoAvailable;
    QString fecha;
    QString ErrorStr;
    QString dbname;

signals:
    void Error(const int ErrType,const QString &Msg,const QString &OrgMsg);
    
public slots:
    
};

#endif // DATABASEMANAGER_H
