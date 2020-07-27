#include "databasemanager.h"
#include <QFile>
#include <QBuffer>
#include <qtiocompressor.h>
#include <QVariant>
#include <QSqlError>
#include <CommonDefines.h>
#include <QSqlRecord>
#include <QUuid>


#define CreateInfSt "CREATE TABLE info (fecha varchar(255),wikicode integer ,compressed integer,NumArts integer);"

DatabaseManager::DatabaseManager(const QString &dbname,QObject *parent) :
    QObject(parent)
{
    DbInfoAvailable = false;

    wikitaxi = false;
    this->dbname = dbname;

}

bool DatabaseManager::CreateDb(){
    bool ret = false;
    if (db.isOpen())
    {
        QSqlQuery("PRAGMA page_size = 4096",db);
        QSqlQuery("PRAGMA cache_size = 16384",db);
        QSqlQuery("PRAGMA temp_store = MEMORY",db);

        //There are other pragmas that influence performance as well.

        QSqlQuery("PRAGMA journal_mode = OFF",db);
        QSqlQuery("PRAGMA legacy_file_format=off",db);
        QSqlQuery("PRAGMA locking_mode = EXCLUSIVE",db);
        QSqlQuery("PRAGMA synchronous = OFF",db);


        QSqlQuery query(db);

        ret = query.exec("create table wikitext (old_id INTEGER PRIMARY KEY autoincrement,old_text mediumblob);");

        if(!ret){
            ErrorStr = "No se pudo ejecutar la consulta."+query.lastError().text();
            emit Error(ErrBd,"Error al crear la fuente documental.",ErrorStr);
            return false;
        }

        ret = query.exec(CreateInfSt);

        if(!ret){
            ErrorStr = "No se pudo ejecutar la consulta."+query.lastError().text();
            emit Error(ErrBd,"Error al crear la fuente documental.",ErrorStr);
            return false;
        }

        ret = query.exec("INSERT INTO info VALUES (strftime('%d-%m-%Y','now'),0,0,0);");

        if(!ret){
            ErrorStr = "No se pudo ejecutar la consulta."+query.lastError().text();
            emit Error(ErrBd,"Error al crear la fuente documental.",ErrorStr);
            return false;
        }

        ret = query.exec("CREATE VIRTUAL TABLE  searchindex USING fts3();");


        if(!ret){
            ErrorStr = "No se pudo ejecutar la consulta."+query.lastError().text();
            emit Error(ErrBd,"Error al crear la fuente documental.",ErrorStr);
            return false;
        }

    } else {
        ErrorStr = "La fuente documental está cerrada.";
        emit Error(ErrFile,"Error al crear la fuente documental.",ErrorStr);
    }
    return ret;
}

bool DatabaseManager::OpenDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE",QUuid::createUuid().toString());
    db.setDatabaseName(this->dbname);
    // Open databasee
    if(db.open()){
        return true;
    }

    ErrorStr = db.lastError().text();
    emit Error(ErrBd,"Error al abrir la fuente documental.",ErrorStr);
    return false;
}

void DatabaseManager::CloseDb()
{
    db.close();
}

QSqlError DatabaseManager::LastErrorSql()
{
    return db.lastError();
}

QString DatabaseManager::LastErrorStr()
{
    return ErrorStr;
}

bool DatabaseManager::DeleteDB(bool ReportErr)
{
    // Close database
    db.close();
    // Remove created database binary file
    if(QFile::exists(this->dbname)){
        if(QFile::remove(this->dbname)){
            return true;
        } else {
            if(ReportErr){
                ErrorStr = "El fichero no se pudo eliminar.\n";
                emit Error(ErrFile,"Error al eliminar la fuente documental.",ErrorStr);
            }
            return false;
        }
    } else {
        if(ReportErr){
            ErrorStr = "El fichero no existe.";
            emit Error(ErrFile,"Error al eliminar la fuente documental.",ErrorStr);
        }
        return false;
    }

    return true;
}

bool DatabaseManager::Insert(QString title, QByteArray text)
{
    bool ret = false;

    if (db.isOpen())
    {

        QSqlQuery query(db);
        query.prepare("INSERT INTO searchindex(content) VALUES(?);");
        query.addBindValue(QVariant(title));

        ret = query.exec();
        if (!ret)
        {
            QString temp = "Error al insertar título:%1.\n";
            ErrorStr = temp.arg(title) + query.lastError().driverText();
            emit Error(ErrBd,"Error al insertar datos en la fuente documental.",ErrorStr);
            return false;
        }


        query.prepare("INSERT INTO wikitext(old_id,old_text) VALUES(NULL,?);");
        query.addBindValue(QVariant(text));

        ret = query.exec();
        if (!ret)
        {
            ErrorStr = "Error al insertar contenido.\n" + query.lastError().text();
            emit Error(ErrBd,"Error al insertar datos en la fuente documental.",ErrorStr);
            return false;
        }

    }
    return ret;
}

bool DatabaseManager::Insert(QString title, QString text)
{
    bool ret = false;

    if (db.isOpen())
    {
        QSqlQuery query(db);

        query.prepare("INSERT INTO searchindex(content) VALUES(?);");
        query.addBindValue(QVariant(title));

        ret = query.exec();


        if (!ret)
        {
            QString temp = "Error al insertar título:%1.\n";
            ErrorStr = temp.arg(title) + query.lastError().driverText();
            emit Error(ErrBd,"Error al insertar datos en la fuente documental.",ErrorStr);
            return false;
        }

        QBuffer data;
        QtIOCompressor compressor(&data);
        compressor.open(QIODevice::WriteOnly);
        compressor.write(text.toUtf8());
        compressor.close();

        query.prepare("INSERT INTO wikitext(old_id,old_text) VALUES(NULL,?);");
        query.addBindValue(QVariant(data.data()));

        ret = query.exec();
        if (!ret)
        {
            ErrorStr = "Error al insertar contenido.\n" + query.lastError().text();
            emit Error(ErrBd,"Error al insertar datos en la fuente documental.",ErrorStr);
            return false;
        }
        // Get database given autoincrement value


    }
    return ret;
}

bool DatabaseManager::IsOpen(){
    return db.isOpen();
}


QStringList DatabaseManager::getTables()
{
    if(db.isOpen()){
        QString strQuery =  "SELECT name FROM sqlite_master"
                " WHERE type='table'"
                " ORDER BY name;";
        QStringList Tables;
        QSqlQuery query(strQuery,db);

        while (query.next()) {
            QString name = query.value(0).toString();
            Tables.append(name);
        }

        if(query.lastError().type() != QSqlError::NoError){
            ErrorStr = query.lastError().text();
            emit Error(ErrBd,"Error al obtener las tablas",ErrorStr);
            return QStringList();
        }
        return  Tables;
    } else {
        ErrorStr = "La fuente documental no está abierta.";
        emit Error(ErrBd,"Error al obtener las tablas.",ErrorStr);
    }

    return QStringList();

}

bool DatabaseManager::IsSqliteDb()
{
    QFile file(this->dbname);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray DbHeaderf = file.read(16);
        QByteArray DbHeader("53514c69746520666f726d6174203300");
        if(!(DbHeaderf.toHex() == DbHeader)){
            ErrorStr = "La fuente documental está dañada o tiene un formato inválido.";
            emit Error(ErrFile,"Error al verificar la fuente documental.",ErrorStr);
            return false;
        }
    } else {
        ErrorStr="Error al abrir la fuente documental.\n"+file.errorString();
        emit Error(ErrFile,"Error verificar la fuente documental.",ErrorStr);
        return false;
    }

    return true;
}

bool DatabaseManager::IsWikiTaxiDb()
{
    QFile file(this->dbname);
    bool ret = false;
    if (file.open(QIODevice::ReadOnly)){

        QByteArray WikiTaxiDbHeader("57696b695461786920444220312e3100");
        QByteArray DbHeaderf = file.read(16);

        if(DbHeaderf.toHex() == WikiTaxiDbHeader){
            ret = true;
        }

    }  else {
        ErrorStr = file.errorString();
        emit Error(ErrFile,"Error al verificar fuente documental.",ErrorStr);
    }

    file.close();
    return ret;
}


bool DatabaseManager::SetWikitaxiHeader()
{
    QFile file(this->dbname);
    if (file.open(QIODevice::ReadWrite)){
        //  QByteArray DbHeaderf = file.read(16);   ver para que sirve
        if(this->IsSqliteDb()){
            return true;
        }

        if(!this->IsWikiTaxiDb()){
            return false;
        }

        QDataStream out(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        file.seek(0);
        out << (quint8)0x53 ;
        out << (quint8)0x51 ;
        out << (quint8)0x4c ;
        out << (quint8)0x69 ;
        out << (quint8)0x74 ;
        out << (quint8)0x65 ;
        out << (quint8)0x20 ;
        out << (quint8)0x66 ;
        out << (quint8)0x6f ;
        out << (quint8)0x72 ;
        out << (quint8)0x6d ;
        out << (quint8)0x61 ;
        out << (quint8)0x74 ;
        out << (quint8)0x20 ;
        out << (quint8)0x33 ;
        out << (quint8)0x00 ;
    }

    if(file.error()!=QFile::NoError){
        ErrorStr = file.errorString();
        emit Error(ErrBd,"Error al poner cabeceras a la fuente documental.",ErrorStr);
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool DatabaseManager::GetDbInfo(DatabaseInfo &dbinfo)
{
    DbInfoAvailable = false;
    if(db.isOpen()){
        if(this->getTables().indexOf("info") == -1){
            dbinfo.setTableAvailable(false);
            ErrorStr = "La tabla de información no está disponible";
            emit Error(ErrBd,"Error al obtener la información de la fuente documental.",ErrorStr);
            return false;
        } else {
            dbinfo.setTableAvailable(true);
        }

        qint32 NumFields = 0;
        QSqlQuery QCountFields(db);

        if(QCountFields.exec("PRAGMA table_info(info);")){
            while(QCountFields.next()){
                NumFields++;
            }
            dbinfo.setNumFields(NumFields);
        } else {
            ErrorStr = QCountFields.lastError().text();
            emit Error(ErrBd,"Error al obtener la información de la fuente documental.",ErrorStr);
            return false;
        }

        QSqlQuery query("SELECT * FROM info;",db);

        while (query.next()) {
            dbinfo.setFecha(query.value(0).toString());
            if(NumFields==4){
                dbinfo.setWikiCode(query.value(1).toBool());
                dbinfo.setCompressed(query.value(2).toBool());
                dbinfo.setNumArts(query.value(3).toULongLong());
            }
        }

        if(query.lastError().type() != QSqlError::NoError){
            ErrorStr = query.lastError().text();
            emit Error(ErrBd,"Error al obtener la información de la fuente documental.",ErrorStr);
            return false;
        }

        if(NumFields<4){
            QSqlQuery query1("SELECT count(*) FROM searchindex;",db);

            while (query1.next()) {
                dbinfo.setNumArts(query1.value(0).toULongLong());
            }

            if(query1.lastError().type() != QSqlError::NoError){
                ErrorStr = query1.lastError().text();
                emit Error(ErrBd,"Error al obtener la información de la fuente documental.",ErrorStr);
                return false;
            }
        }

        DbInfoAvailable = true;

    } else {
        ErrorStr =  "la fuente documental está cerrada.";
        emit Error(ErrFile,"Error al obtener la información de la fuente documental.",ErrorStr);
        return false;
    }
    return true;
}


bool DatabaseManager::SetDbInfo(bool wikicode, bool compressed, quint64 NumArts)
{
    DbInfoAvailable = false;
    unsigned char WikiCodeValue = 0;
    unsigned char CompressedValue = 0;

    if(wikicode){
        WikiCodeValue = 1;
    }

    if(compressed){
        CompressedValue = 1;
    }

    if(db.isOpen()){

        QSqlQuery("PRAGMA page_size = 4096",db);
        QSqlQuery("PRAGMA cache_size = 16384",db);
        QSqlQuery("PRAGMA temp_store = MEMORY",db);

        //There are other pragmas that influence performance as well.

        QSqlQuery("PRAGMA journal_mode = OFF",db);
        QSqlQuery("PRAGMA legacy_file_format=off",db);
        QSqlQuery("PRAGMA locking_mode = EXCLUSIVE",db);
        QSqlQuery("PRAGMA synchronous = OFF",db);

        QStringList Tables = this->getTables();

        if(Tables.length()==0){
            ErrorStr = "No hay tablas disponibles.";
            emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
            return false;
        }

        if(Tables.indexOf("info")==-1){
            //create table

            QSqlQuery q(db);

            if(!q.exec(CreateInfSt)){
                ErrorStr = "No se pudo crear la tabla. \n" + q.lastError().text();
                emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                return false;
            }

        } else {
            //verifying number of fields
            qint32 NumFields = 0;
            QSqlQuery QCountFields(db);

            if(QCountFields.exec("PRAGMA table_info(info);")){
                while(QCountFields.next()){
                    NumFields++;
                }

                if(NumFields < 4){
                    //drop and create table

                    QSqlQuery q(db);

                    if(!q.exec("drop table info;")){
                        ErrorStr = "No se pudo eliminar la tabla.\n" + q.lastError().text();
                        emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                        return false;
                    }

                    if(!q.exec(CreateInfSt)){
                        ErrorStr = "No se pudo crear la tabla.\n" + q.lastError().text();
                        emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                        return false;
                    }

                } else {
                    //truncate fields
                    QSqlQuery TruncateQuery(db);
                    if(!TruncateQuery.exec("DELETE FROM info")){
                        ErrorStr = "No se pudo vaciar la tabla.\n" + TruncateQuery.lastError().text();
                        emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                        return false;
                    }


                }

            } else {
                //Err
                ErrorStr = "No se pudo obtener información de la tabla.\n" + QCountFields.lastError().text();
                emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                return false;
            }

        }

        //indexOf

        if(NumArts==0){
            QSqlQuery query1(db);
            //QString QueryCountStr;
            QString QueryCountStr = "SELECT count(*) FROM searchindex;";
            qint64 Cantida_art=0;

            if(query1.lastError().type() != QSqlError::NoError){
                ErrorStr = "Error al ejecutar la consulta.\n" + QueryCountStr + " "+ query1.lastError().text();
                emit Error(ErrBd,"Error al obtener la información de la fuente documental.",ErrorStr);
                return false;
            }
            while (query1.next()) {
                Cantida_art = query1.value(0).toLongLong();
            }


            QString insert = QString("INSERT INTO info VALUES (strftime('%d-%m-%Y','now'),%1,%2,%3);")
                    .arg(WikiCodeValue).arg(CompressedValue).arg(Cantida_art);


            if(!query1.exec(insert)){
                ErrorStr = "No se pudo ejecutar la consulta de actualización 1.\n" + query1.lastError().text() ;
                emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                return false;
            }




        } else {
            //update

            QString insert = QString("INSERT INTO info VALUES (strftime('%d-%m-%Y','now'),%1,%2,%3);")
                    .arg(WikiCodeValue).arg(CompressedValue).arg(NumArts);

            QSqlQuery query(db);
            if(!query.exec(insert)){
                ErrorStr = "No se pudo ejecutar la consulta de actualización.\n" + query.lastError().text();
                emit Error(ErrBd,"Error al actualizar la información de la fuente documental.",ErrorStr);
                return false;
            }
        }

        DbInfoAvailable = true;

    } else {
        ErrorStr = "la fuente documental está cerrada.";
        emit Error(ErrFile,"Error al obtener la información de la fuente documental.",ErrorStr);
        return false;
    }

    return true;
}


QString DatabaseInfo::Fecha() const {
    return this->p_Fecha;
}
void DatabaseInfo::setFecha(const QString& Fecha){
    this->p_Fecha = Fecha;
}

bool DatabaseInfo::WikiCode() const{
    return this->p_WikiCode;
}

void DatabaseInfo::setWikiCode(const bool& WikiCode){
    this->p_WikiCode = WikiCode;
}

bool DatabaseInfo::Compressed() const{
    return this->p_Compressed;
}

void DatabaseInfo::setCompressed(const bool& Compressed){
    this->p_Compressed=Compressed;
}

bool DatabaseInfo::IsTableAvailable() const{
    return this->p_IsTableAvailable;
}

void DatabaseInfo::setTableAvailable(const bool& IsTableAvailable){
    this->p_IsTableAvailable=IsTableAvailable;
}

quint64 DatabaseInfo::NumArts() const{
    return this->p_NumArts;
}


void DatabaseInfo::setNumArts(const quint64& NumArts){
    this->p_NumArts=NumArts;
}



qint32 DatabaseInfo::NumFields() const{
    return this->p_NumFields;
}

void DatabaseInfo::setNumFields(const qint32& NumFields){
    this->p_NumFields=NumFields;
}

qint32 DatabaseInfo::WebServerPort() const
{
    return this->p_WebServerPort;
}

void DatabaseInfo::setWebServerPort(const qint32 &WebServerPort)
{
    this->p_WebServerPort=WebServerPort;
}

QStringList DatabaseInfo::FieldNames() const
{
	return this->p_FieldNames;
}

void DatabaseInfo::setFieldNames(const QStringList& FieldNames)
{
	this->p_FieldNames=FieldNames;
}
	
//DatabaseInfo::DatabaseInfo(const DatabaseInfo &other)
//{

//    this->p_WikiCode=other.p_WikiCode;
//    this->p_Compressed=other.p_Compressed;
//    this->p_IsTableAvailable=other.p_IsTableAvailable;
//    this->p_NumArts=other.p_NumArts;
//    this->p_NumFields=other.p_NumFields;
//    this->p_WebServerPort=other.p_WebServerPort;
//    this->p_Fecha=other.p_Fecha;

//}







