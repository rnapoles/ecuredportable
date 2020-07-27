#ifndef DBQUERY_H
#define DBQUERY_H

#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

class DBQuery : public QObject
{
    Q_OBJECT
public:
    explicit DBQuery(const QString& dbname,QObject *parent = 0);
    ~DBQuery();
    void execute( const QString& query );

    void start();
    void terminate();
    bool wait(unsigned long time = ULONG_MAX);

signals:

  void results( const QList<QSqlRecord>& records ,double tiempo);
  void Error(const int ErrType,const QString Msg,const QString OrgMsg);
    
public slots:

private:
  QString database;
  QSqlDatabase m_database;
  void ThrowError(const QString Msg,const QSqlError err);
    
};

#endif // DBQUERY_H
