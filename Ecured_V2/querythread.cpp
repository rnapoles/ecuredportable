#include "querythread.h"
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <ctime>
#include "appconfig.h"

QMutex Worker::mutex;

Worker::Worker( const QString& dbname , QObject* parent )
    : QObject( parent )
{
    database = dbname;
}

Worker::~Worker()
{

}

void Worker::slotExecute( const QString& query )
{
    mutex.lock();
    //QMutexLocker lock(&mutex);

    //{
        static  unsigned int init = 0;
        if(init==0){
            m_database = QSqlDatabase::addDatabase("QSQLITE",database);
            m_database.setDatabaseName(AppConfig::instance()->getWikiDb());
            init++;
        }


        if (!m_database.isOpen())
            if (!m_database.open())
            {
                ThrowError("No ha sido posible abrir la base de datos:",m_database.lastError());
                mutex.unlock();
                return ;
            }

        if(m_database.isOpen()){
            QList<QSqlRecord> recs;
            clock_t t1,t2;
            t1 = clock();
            QSqlQuery sql(query,m_database);
            //qDebug()<<query;
            QSqlError err = sql.lastError();
            if(err.type()!=QSqlError::NoError){
                ThrowError("Error en la consulta",sql.lastError());
                mutex.unlock();
                return ;
            }
            while ( sql.next() )
            {
                recs.push_back( sql.record() );
            }

            t2 = clock();
             double tiempo = double( t2 - t1 ) / (double)CLOCKS_PER_SEC;
             emit results( recs ,tiempo);
             //qDebug()<<recs;
        } else {

            ThrowError("Base de datos inválida",m_database.lastError());
            mutex.unlock();
            return ;
        }
    //}

    //QSqlDatabase::removeDatabase(database);
    mutex.unlock();
    //delete this;
}

void Worker::ThrowError(const QString Msg,const QSqlError err){
    QList<QSqlRecord> recs;
    emit Error(1,Msg,err.text());
    emit results( recs ,0);
}

////

QueryThread::QueryThread(const QString& dbname , QObject *parent)
    : QThread(parent)
{
    setTerminationEnabled();
    database = dbname;
}

QueryThread::~QueryThread()
{
    delete m_worker;
    QSqlDatabase::removeDatabase(database);

}

void QueryThread::execute( const QString& query )
{
    emit executefwd( query ); // forwards to the worker
}


void QueryThread::run()
{
    m_worker = new Worker(database);

    connect( this, SIGNAL( executefwd( const QString& ) ), m_worker, SLOT( slotExecute( const QString& ) ));

    // Critical: register new type so that this signal can be
    // dispatched across thread boundaries by Qt using the event
    // system
    //qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );

    // forward final signal
    connect(m_worker, SIGNAL( results(QList<QSqlRecord>,double)),this, SIGNAL( results(QList<QSqlRecord>,double)));
    connect(m_worker,SIGNAL(Error(int,QString,QString)),this,SIGNAL(Error(int,QString,QString)));

    exec();  // our event loop
}



//#include "querythread.moc"
