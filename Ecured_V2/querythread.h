#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QList>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>


//http://www.linuxjournal.com/article/9602

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker( const QString& dbname , QObject* parent = 0);
    ~Worker();

public slots:
    void slotExecute( const QString& query );

signals:
    void results( const QList<QSqlRecord>& records ,double tiempo);
    void Error(const int ErrType,const QString Msg,const QString OrgMsg);

private:
    static QMutex mutex;
    QString database;
    QSqlDatabase m_database;
    void ThrowError(const QString Msg,const QSqlError err);
};


class QueryThread : public QThread
{
  Q_OBJECT

  public: 
    QueryThread(const QString& dbname,QObject *parent = 0);
    ~QueryThread();

    void execute( const QString& query );
 
  signals:
    void results( const QList<QSqlRecord>& records ,double tiempo);
    void executefwd( const QString& query );
    void Error(const int ErrType,const QString Msg,const QString OrgMsg);

  protected:
    void run();

  private:
     Worker* m_worker;
     QString database;
};



// The class that does all the work with the database. This class will
// be instantiated in the thread object's run() method.


#endif
