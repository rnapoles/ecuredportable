#include "indexcontroller.h"
#include "template.h"
#include "static.h"
#include <ctime>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QUuid>
#include "databasemanager.h"
#include "util.h"

struct ExtensionMap {
    const char *extension;
    const char *mimeType;
} extensionMap[] = {
{ ".bmp", "image/bmp" },
{ ".css", "text/css" },
{ ".gif", "image/gif" },
{ ".html", "text/html" },
{ ".htm", "text/html" },
{ ".ico", "image/x-icon" },
{ ".jpeg", "image/jpeg" },
{ ".jpg", "image/jpeg" },
{ ".js", "application/x-javascript" },
{ ".mng", "video/x-mng" },
{ ".pbm", "image/x-portable-bitmap" },
{ ".pgm", "image/x-portable-graymap" },
{ ".pdf", "application/pdf" },
{ ".png", "image/png" },
{ ".ppm", "image/x-portable-pixmap" },
{ ".rss", "application/rss+xml" },
{ ".svg", "image/svg+xml" },
{ ".svgz", "image/svg+xml" },
{ ".text", "text/plain" },
{ ".tif", "image/tiff" },
{ ".tiff", "image/tiff" },
{ ".txt", "text/plain" },
{ ".xbm", "image/x-xbitmap" },
{ ".xml", "text/xml" },
{ ".xpm", "image/x-xpm" },
{ ".xsl", "text/xsl" },
{ ".xhtml", "application/xhtml+xml" },
{ ".wml", "text/vnd.wap.wml" },
{ ".wmlc", "application/vnd.wap.wmlc" },
{ "about:blank", 0 },
{ 0, 0 }
};

IndexController::IndexController(QObject*parent){

}

void IndexController::service(HttpRequest &request, HttpResponse &response)
{
    QString path(request.getPath());
    QString mimeType = mimeFromUrl(path);

    if(!mimeType.isEmpty()){
        Static::staticFileController->service(request, response);
        return;
    }

    QMultiMap<QByteArray,QByteArray> ParameterMap = request.getParameterMap();


    bool issearch = false;

    if (ParameterMap.count() > 0){

        issearch = true;
        if(ParameterMap.contains("id")){
            this->SearchById(request,response,ParameterMap.value("id"));
            return;
        }
        if (ParameterMap.contains("buscar")){
            //            search = queryItems.at(0).second;
            //            search = search.replace('_',' ');
            //            if(queryItems.count()==2){
            //                if(queryItems.at(1).first.compare("useMatch")==0){
            //                    SearchEngine::instance()->SearchByTitle(search,true);
            //                } else {
            //                    SearchEngine::instance()->SearchByTitle(search);
            //                }
            //            }else {
            //                this->SearchByTitle(request,response,search);
            //            }

            this->SearchByTitle(request,response,ParameterMap.value("buscar"));
            return;
        }

    }

    if(path!="/index/"){
        search = path.remove("/index/");
        search = search.replace('_',' ');
        this->SearchByTitle(request,response,search);
        return;
    }

    response.setHeader("Content-Type", "text/html; charset=UTF-8");

    Template t=Static::templateLoader->getTemplate("index",request.getHeader("Accept-Language"));
    t.enableWarnings();
    t.setVariable("path",request.getPath());
    response.write(t.toUtf8(),true);

}

void IndexController::redirect(HttpRequest &request, HttpResponse &response)
{
    response.setStatus(302,"redirect");
    response.setHeader("Location","/index/");
}

QString IndexController::mimeFromUrl(const QString &path)
{

    const int index = path.lastIndexOf(QLatin1Char('.'));
    const QByteArray &ext = path.mid(index).toUtf8().toLower();

    const ExtensionMap *e = extensionMap;
    while (e->extension) {
        if (ext == e->extension)
            return QLatin1String(e->mimeType);
        ++e;
    }
    return QLatin1String("");
}

void IndexController::SearchByTitle(HttpRequest &request, HttpResponse &response, const QString &str, bool useMatch)
{
    QString query =
            "SELECT * FROM "
            " (SELECT content,old_id,old_text FROM"
            " wikitext JOIN searchindex ON old_id=docid"
            " WHERE content MATCH '"+str+"')"
            " WHERE content LIKE '"+str+"' LIMIT 1";
    this->RunQuery(query,request,response);
}

void IndexController::SearchById(HttpRequest &request, HttpResponse &response, const QString &id)
{
    QString query = " SELECT content,old_id,old_text FROM wikitext JOIN searchindex ON old_id=docid where docid = '"+id+"' LIMIT 1";
    this->RunQuery(query,request,response);

}

void IndexController::RunQuery(const QString &query, HttpRequest &request, HttpResponse &response)
{

    QSqlDatabase m_database = QSqlDatabase::addDatabase("QSQLITE",QUuid::createUuid().toString());
    m_database.setDatabaseName("wiki.db");


    if (!m_database.isOpen())
        if (!m_database.open())
        {
            ThrowError(request,response,"No ha sido posible abrir la base de datos: %1",m_database.lastError());
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
            ThrowError(request,response,"Error en la consulta: %1",sql.lastError());
            return ;
        }
        while ( sql.next() )
        {
            recs.push_back( sql.record() );
        }

        t2 = clock();
        double tiempo = double( t2 - t1 ) / (double)CLOCKS_PER_SEC;
        //emit results( recs ,tiempo);
        this->ProcessResults(request,response,recs,tiempo);
    } else {
        ThrowError(request,response,"Base de datos inválida: %1",m_database.lastError());
    }
}

void IndexController::ThrowError(HttpRequest &request, HttpResponse &response, const QString Msg, const QSqlError err)
{

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=Static::templateLoader->getTemplate("error",request.getHeader("Accept-Language"));
    t.enableWarnings();
    t.setVariable("texto",Msg.arg(err.text()));
    t.setVariable("version",QCoreApplication::applicationVersion());
    response.write(t.toUtf8(),false);
}

void IndexController::ProcessResults(HttpRequest &request, HttpResponse &response, const QList<QSqlRecord> &records, double tiempo)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    int size = records.size();
    if(size > 0){
        DatabaseInfo dbinfo;
        DatabaseManager dbm("wiki.db");

        if(dbm.OpenDB()){
            dbm.GetDbInfo(dbinfo);
            dbm.CloseDb();
        }

        QByteArray datos,texto;
        datos =  records.at(0).value(2).toByteArray();
        QString title = records.at(0).value(0).toString();

        bool CompressionError=false;
        datos = gUncompress(datos,CompressionError);
        if(CompressionError==false){
            texto=datos;
        }


        Template t=Static::templateLoader->getTemplate("contenedor");
        t.enableWarnings();
        t.setVariable("texto",texto);

        if(dbinfo.WikiCode()){
            t.setVariable("wikicode","var wikicode = true");
        } else {
            t.setVariable("wikicode","var wikicode = false");
        }

        t.setVariable("version",QCoreApplication::applicationVersion());
        t.setVariable("title",Util::ucFirst(title.toUtf8()));
        response.write(t.toUtf8(),false);

    } else {

        QString texto(request.getPath());
        texto = texto.remove("/index/");
        texto = texto.replace('_',' ');

        Template t=Static::templateLoader->getTemplate("not",request.getHeader("Accept-Language"));
        t.enableWarnings();
        t.setVariable("texto",texto);
        t.setVariable("version",QCoreApplication::applicationVersion());
        response.write(t.toLatin1(),false);
    }
}
