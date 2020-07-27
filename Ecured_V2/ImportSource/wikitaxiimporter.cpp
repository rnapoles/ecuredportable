#include "wikitaxiimporter.h"
#include <databasemanager.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <CommonDefines.h>
#include <QBuffer>
#include <qtiocompressor.h>

WikiTaxiImporter::WikiTaxiImporter(QObject *parent) :
    QObject(parent)
{
}

void WikiTaxiImporter::SetSource(const QString &s)
{
    this->source=s;
}

void WikiTaxiImporter::SetDestination(const QString &s)
{
    this->destination=s;
}

void WikiTaxiImporter::copy()
{
    emit Init();

    DatabaseManager dbm(this->source);
    if(!dbm.IsWikiTaxiDb()){
        emit Error(ErrFile,"La fuente documental selecionada no es válida o esta dañada.","");
        emit Finish();
        return;
    }

    DatabaseManager dbm1(this->destination);
    connect(&dbm1,SIGNAL(Error(int,QString,QString)),this,SIGNAL(Error(int,QString,QString)));
    if(dbm1.OpenDB()){
        if(!dbm1.CreateDb()){
            emit Finish();
            return;
        }
    } else {
        emit Finish();
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("WIKITAXIDB","wikitaxi");
    db.setDatabaseName(this->source);
    if(db.open()){

        int start = 0;
        int size = 100;
        int contador = 0;
        int total = -1;

        QSqlQuery query(db);

        if(query.exec("SELECT count(docid) FROM \"n_content\";")){
            while (query.next()) {
                total = query.value(0).toInt();
            }
            emit FileSize(total);
        } else {
            emit Error(ErrBd,"Error en la consulta.",query.lastError().text());
            emit Finish();
            return;
        }

        QString strQuery;

        while (contador < total ){

            strQuery = QString("SELECT c0n_value,t_text FROM  t JOIN n_content ON t_id=docid LIMIT %1,%2;")
                    .arg(start).arg(size);
            if(query.exec(strQuery)){
                while (query.next()){
                    QString Title = query.value(0).toString();
                    QByteArray byteArray = query.value(1).toByteArray();
                    QBuffer DataBuffer(&byteArray);
                    QtIOCompressor compressor(&DataBuffer);
                    compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
                    compressor.open(QIODevice::ReadOnly);
                    const QByteArray Text = compressor.readAll();


                    if(!dbm1.Insert(Title,QString(Text))){                        
                        dbm1.CloseDb();
                        emit Finish();
                        return;
                    }
                    contador++;

                }
                start += size;
                emit ReportProgress(contador);
            } else {
                emit Error(ErrBd,"Error al insertar datos en la fuente documental.",query.lastError().text());
                dbm1.CloseDb();
                emit Finish();
                return;
            }
        }

        if(!dbm1.SetDbInfo(true,true,contador)){          
            dbm1.CloseDb();
            emit Finish();
            return;
        }

    } else {
        dbm1.CloseDb();
        emit Finish();
        return;
    }

    dbm1.CloseDb();
    emit Finish();

}
