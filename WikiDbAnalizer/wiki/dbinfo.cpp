#include "dbinfo.h"
#include <QSqlDatabase>
#include <QSqlQuery>

DbInfo *DbInfo::s_instance = NULL;
DbInfo::DbInfo()
{
    //QMutex m;
    //m.lock();
    updateInfo();
    //m.unlock();
}

QString DbInfo::getFecha()  {

    return pfecha;
}


QString DbInfo::getCantidadArticulos()  {

    return pCantida_art;
}

 void DbInfo::updateInfo(){

     {

         QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","info");
         db.setDatabaseName("wikidb");
         if(db.open()){
             QSqlQuery query("SELECT * FROM info;",db);

             while (query.next()) {
                 pfecha = query.value(0).toString();
             }

             QSqlQuery query1("SELECT count(*) FROM searchindex;",db);

             while (query1.next()) {
                 pCantida_art = query1.value(0).toString();
             }

         }
     }


      QSqlDatabase::removeDatabase("info");

 }
