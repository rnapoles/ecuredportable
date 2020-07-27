#include <ctime>
#include <QSqlError>
#include <QSqlQuery>
#include "appconfig.h"
#include <QDebug>

#include "dbquery.h"
#include <QMetaType>

DBQuery::DBQuery(const QString &dbname,QObject *parent) :
    QObject(parent)
{
     this->database = dbname;
}

DBQuery::~DBQuery()
{
}

void DBQuery::execute(const QString &query)
{
    qDebug()<<"DBQuery::execute";
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
            return ;
        }

    if(m_database.isOpen()){
        QList<QSqlRecord> recs;
        clock_t t1,t2;
        t1 = clock();
        QSqlQuery sql(query,m_database);
        qDebug()<<query;
        QSqlError err = sql.lastError();
        if(err.type()!=QSqlError::NoError){
            ThrowError("Error en la consulta",sql.lastError());
            return ;
        }
        while ( sql.next() )
        {
            //qDebug()<<sql.record();
            recs.push_back( sql.record() );
        }

        t2 = clock();
        double tiempo = double( t2 - t1 ) / (double)CLOCKS_PER_SEC;
        emit results( recs ,tiempo);
         //qDebug()<<recs;
    } else {

        ThrowError("Base de datos inválida",m_database.lastError());
        return ;
    }
}

void DBQuery::start()
{
}

void DBQuery::terminate()
{
}

bool DBQuery::wait(unsigned long time)
{
    return true;
}

void DBQuery::ThrowError(const QString Msg, const QSqlError err)
{
    QList<QSqlRecord> recs;
    emit Error(1,Msg,err.text());
    emit results(recs ,0);
}


