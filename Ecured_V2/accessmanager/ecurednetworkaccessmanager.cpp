#include "ecurednetworkaccessmanager.h"
#include <QTimer>
//#include <QFuture>
#include <QtConcurrentRun>
#include <QFile>
#include <QWebElement>
#include <QDesktopServices>
#include <QBuffer>
#include <QCoreApplication>
#include <databasemanager.h>
#include "util.h"
#include "appconfig.h"



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



EcuredNetworkReply::EcuredNetworkReply(const QNetworkRequest &request,
                                       const QByteArray &fileData, const QString& mimeType)
    : QNetworkReply(),data(fileData), origLen(fileData.length())
{

    view = NULL;
    setRequest(request);
    setOpenMode(QIODevice::ReadOnly);

    setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
    QTimer::singleShot(1, this, SIGNAL(metaDataChanged()));
    QTimer::singleShot(2, this, SIGNAL(readyRead()));
}


EcuredNetworkReply::EcuredNetworkReply(const QNetworkRequest &request)
    : QNetworkReply()
{

    QUrl url = request.url();
    //qDebug()<<url;
    static int counter ;
    counter++;
    setOpenMode(QIODevice::ReadOnly);
    wikihelper = new WikiHelper();
    view = new QWebView();
    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    QList<QPair<QString, QString> > queryItems = url.queryItems();
    bool issearch = false;

    connect(SearchEngine::instance(),SIGNAL(OnSearchByTitleReady(QList<QSqlRecord>,bool,double)),this,SLOT(OnSearchByTitleReady(QList<QSqlRecord>,bool,double)));


    connect(SearchEngine::instance(),SIGNAL(OnSearchByTitleReady(QList<QPair<QString,int> >,double)),this,SLOT(OnSearchByTitleReady(QList<QPair<QString,int> >,double)));
    //connect(SearchEngine::instance(),SIGNAL(SearchStarted()),this,SLOT(test()));

    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(on_frame_loadFinished(bool)));

    QFile html;
    html.setFileName("recursos/css/plantilla/general.html");
    html.open(QIODevice::ReadOnly);
    QString Htmlstr  = html.readAll();
    view->setHtml(Htmlstr);


    if (queryItems.count() > 0){

        issearch = true;
        if(queryItems.at(0).first.compare("id")==0){
            SearchEngine::instance()->SearchById(queryItems.at(0).second.toInt());
        } else {
            search = queryItems.at(0).second;
            search = search.replace('_',' ');
            if(queryItems.count()==2){
                if(queryItems.at(1).first.compare("useMatch")==0){

                    SearchEngine::instance()->SearchByTitle(search,true);
                } else {

                    SearchEngine::instance()->SearchByTitle(search);
                }
            }else {

                SearchEngine::instance()->SearchByTitle(search);
            }
        }

    }

    if(!url.path().isEmpty() && issearch==false){
        search = url.path().remove('/');
        search = search.replace('_',' ');
        SearchEngine::instance()->SearchByTitle(search);
    }




}

EcuredNetworkReply::~EcuredNetworkReply(){

    //this->dumpObjectInfo();
    if (view != NULL) {
        delete view;
        delete wikihelper;
    }

}


void EcuredNetworkReply::abort()
{
    //TRACE_OBJ
}

qint64 EcuredNetworkReply::readData(char *buffer, qint64 maxlen)
{
    qint64 len = qMin(qint64(data.length()), maxlen);
    if (len) {
        qMemCopy(buffer, data.constData(), len);
        data.remove(0, len);
    }
    if (!data.length())
        QTimer::singleShot(0, this, SIGNAL(finished()));
    return len;
}



void EcuredNetworkReply::on_frame_loadFinished(bool)
{

    frame = view->page()->mainFrame();

    QFile file;
    file.setFileName(":javascript/monobook.js");

    file.open(QIODevice::ReadOnly);
    WikiMarkupParser = file.readAll();
    file.close();

    view->page()->mainFrame()->evaluateJavaScript(WikiMarkupParser);
    //end wikicode2html

    file.setFileName("recursos/javascript/jquery-latest.js");
    file.open(QIODevice::ReadOnly);
    QString jquery = file.readAll();
    file.close();

    view->page()->mainFrame()->evaluateJavaScript(jquery);

}

void EcuredNetworkReply::OnSearchByTitleReady(const QList<QSqlRecord>& records,bool useMatch,double tiempo ){



    int size = records.size();


    if(size > 0){

        QStringList ListaResultados;

        DatabaseInfo dbinfo;
        DatabaseManager dbm(AppConfig::instance()->getWikiDb());

        if(dbm.OpenDB()){
            dbm.GetDbInfo(dbinfo);
            dbm.CloseDb();
        }

        QByteArray datos;

        for (int i = 0; i < size; ++i) {
            QString texto =  records.at(i).value(2).toString();
            datos =  records.at(i).value(2).toByteArray();
            //qDebug()<<records.at(i).value(2).typeName();
            ListaResultados.append(texto);
            break;
        }


        search = records.at(0).value(0).toString();
        size = ListaResultados.size();


        QWebElement Contenedor = frame->findFirstElement("#IdContenedor");

        Contenedor.setInnerXml("");

        QString str1 = ListaResultados.at(0);

        bool CompressionError=false;
        datos = gUncompress(datos,CompressionError);
        if(CompressionError==false){
            str1=QString(datos);
        }

        wikihelper->setTextContainer(str1);

        if(dbinfo.WikiCode()){
            QString str = frame->evaluateJavaScript("wiki2html(wikihelper.getTextContainer())").toString();

            Contenedor.setInnerXml(str);

            frame->evaluateJavaScript("AppReady()");

        } else {

            frame->evaluateJavaScript("$('.contenedor_derecho2').remove();");
            Contenedor.setInnerXml(str1);
        }
        QString js =
                "$('.thumbinner').remove();"
                "$('.editsection').remove();"
                "$('.floatright').remove();"
                "$('.floatleft').remove();"
                "$('form').remove();"
                "$('.contenedor_pie_pagina_firma').html('Centro de Desarrollo Territorial Holguín – UCI&nbsp;&nbsp;<br/>Ecured Portable v%1&nbsp;&nbsp;<br/>2011-2012&nbsp;&nbsp;<br/>');"
                ;

        js = js.arg(QCoreApplication::applicationVersion());


        frame->evaluateJavaScript(js);
        frame->evaluateJavaScript("document.title = '"+ Util::ucFirst(search.toUtf8()) +"';");

        data = frame->toHtml().toUtf8();

    } else {

        QFile fProtocolError;
        fProtocolError.setFileName("recursos/css/plantilla/not.html");
        fProtocolError.open(QIODevice::ReadOnly);
        QString notfound = fProtocolError.readAll();


        data = notfound.arg(search).arg(QCoreApplication::applicationVersion()).toUtf8();

    }

    setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));
    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
    QTimer::singleShot(1, this, SIGNAL(metaDataChanged()));
    QTimer::singleShot(2, this, SIGNAL(readyRead()));
}

void EcuredNetworkReply::OnSearchByTitleReady(const QList<QPair<QString, int> >& results,double tiempo){


    int size = results.size();

    if(size > 0){
        SearchEngine::instance()->SearchById(results.at(0).second);
    } else {

        QFile fProtocolError;
        fProtocolError.setFileName("recursos/css/plantilla/not.html");
        fProtocolError.open(QIODevice::ReadOnly);
        QString notfound = fProtocolError.readAll();
        data = notfound.arg(search).arg(QCoreApplication::applicationVersion()).toUtf8();

        setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));
        setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
        QTimer::singleShot(1, this, SIGNAL(metaDataChanged()));
        QTimer::singleShot(2, this, SIGNAL(readyRead()));

    }




}

void EcuredNetworkReply::populateJavaScriptWindowObject()
{
    view->page()->mainFrame()->addToJavaScriptWindowObject("wikihelper", wikihelper);

}

//void EcuredNetworkReply::test()
//{
//    qDebug()<<"ok";
//}



/****************EcuredNetworkAccessManager*********************/

EcuredNetworkAccessManager::EcuredNetworkAccessManager(QObject *parent)
    : QNetworkAccessManager(parent)
{
    setNetworkAccessible(QNetworkAccessManager::NotAccessible);
    connect(this,SIGNAL(finished(QNetworkReply*)),this, SLOT(closeRequest(QNetworkReply*)));
}

EcuredNetworkAccessManager::~EcuredNetworkAccessManager(){

}

QNetworkReply *EcuredNetworkAccessManager::get(const QNetworkRequest &request)
{
    //qDebug()<<"Get "<<request.url();
    return QNetworkAccessManager::get(request);
}

QNetworkReply *EcuredNetworkAccessManager::post(const QNetworkRequest &request, QIODevice *data)
{
    //qDebug()<<"Post "<<request.url();
    return QNetworkAccessManager::post(request,data);
}


QNetworkReply *EcuredNetworkAccessManager::createRequest(Operation op,
                                                         const QNetworkRequest &request, QIODevice* outgoingData)
{
    AppConfig *appConfig = AppConfig::instance();


    url = request.url().toString();

    if(appConfig->IsUsingRemoteServer()){
        setNetworkAccessible(QNetworkAccessManager::Accessible);
        if(url.startsWith("http://ecured/")){
            QString NewUrlString = QString("http://%1:%2/").arg(appConfig->getServerIP()).arg(appConfig->getServerPort());
            url = url.replace("http://ecured/",NewUrlString);

            if(!url.contains("/index/")){
                url = url.replace("/index","/index/");
            }

            if(!url.contains("/index/")){

                url = url.replace(NewUrlString,NewUrlString+"index/");
            }

            //qDebug()<<QUrl(url).toEncoded();

            //QUrl NewUrl(NewUrlString);
            //request.setUrl(QUrl(NewUrlString));

            //qDebug()<<"IsUsingRemoteServer";
        }
    } else {
        setNetworkAccessible(QNetworkAccessManager::NotAccessible);
    }

    //qDebug()<<url;

    if(url.startsWith("http://ecured/")){
        //if(request.url().scheme() == "http"){

        QString path = request.url().path();
        if(QFile::exists(":/info"+path)){
            const QString &mimeType = mimeFromUrl(url);
            QFile loaddata;
            loaddata.setFileName(":/info"+path);
            loaddata.open(QIODevice::ReadOnly);
            const QByteArray &data  = loaddata.readAll();

            return new EcuredNetworkReply(request, data, mimeType.isEmpty()? QLatin1String("application/octet-stream") : mimeType);
        }

        if(!request.url().path().isEmpty()){

            const QString &mimeType = mimeFromUrl(url);

            path = "recursos"+path;
            //verificar path de la fuente documental

            if(QFile::exists(path)){
                //Cargando Path
                QFile loaddata;
                loaddata.setFileName(path);
                loaddata.open(QIODevice::ReadOnly);
                const QByteArray &data  = loaddata.readAll();
                return new EcuredNetworkReply(request, data, mimeType.isEmpty()? QLatin1String("application/octet-stream") : mimeType);

            } else if (mimeType != "") {

                QString ProtocolError = "<title>Error 404...</title><div "
                        "align=\"center\"><br><br><h1>The page could not be found</h1><br><h3>'%1'"
                        "</h3></div>";



                const QByteArray &data=ProtocolError.toUtf8();
                EcuredNetworkReply *reply= new EcuredNetworkReply(request, data, mimeType.isEmpty()? QLatin1String("application/octet-stream") : mimeType);
                //reply->setError(QNetworkReply::ContentNotFoundError,"Error 404");
                return reply;

            }
        }


        //qDebug()<<request.url().toString();
        return new EcuredNetworkReply(request);
        // }
    }

    //    if(MainWindow::instance()->IsUsingRemoteServer()){
    //        QString urlToCheck = QString("http://%1:%2/").arg(mainWindow->GetServerIP()).arg(mainWindow->GetServerPort());
    //        if(url.startsWith(urlToCheck)){

    //        }
    //    }

    //QtConcurrent::run(QDesktopServices::openUrl,url);
    return QNetworkAccessManager::createRequest(op, QNetworkRequest(url), outgoingData);
}

QString EcuredNetworkAccessManager::mimeFromUrl(const QUrl &url)
{


    const QString &path = url.path();
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

void EcuredNetworkAccessManager::openExternalUrl(){
    QDesktopServices::openUrl(url);
}


void EcuredNetworkAccessManager::closeRequest( QNetworkReply * reply ){
    //qDebug()<<"Finish reply " << reply->url();
}


