#include "searchengine.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlRecord>
#include <QHashIterator>
#include <QHash>
#include "clucenehelper.h"
#include "appconfig.h"

#include <qobjecthelper.h>
#include <parser.h>
#include "lucenedocument.h"




SearchEngine *SearchEngine::s_instance = 0;

SearchEngine * SearchEngine::instance()
{
    if (!s_instance)
        s_instance = new SearchEngine;
    return s_instance;
}

SearchEngine::SearchEngine( QObject* parent )
    : QObject( parent )
{
    qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );

    this->MultiFieldSearch=false;
    this->SearchField = "Título";

    ManagerSearchByTitle = new QNetworkAccessManager(this);
    ManagerTextCompletion = new QNetworkAccessManager(this);

    connect(ManagerSearchByTitle, SIGNAL(finished(QNetworkReply*)),this, SLOT(SearchByTitleReplyFinished(QNetworkReply*)));
    connect(ManagerTextCompletion, SIGNAL(finished(QNetworkReply*)),this, SLOT(TextCompletionReplyFinished(QNetworkReply*)));

    SearchByTitleQuery = new PQuery("wikidb");
    TextCompletionQuery = new PQuery("wikidbindex");


    connect( TextCompletionQuery,SIGNAL(results(QList<QSqlRecord>,double)),this,SLOT(slotTextCompletion(QList<QSqlRecord>,double)));

    useMatch =false;
    SearchByTitleQuery->start();
    TextCompletionQuery->start();

    connect( SearchByTitleQuery,SIGNAL(results(QList<QSqlRecord>,double)),this,SLOT(slotSearchByTitle(QList<QSqlRecord>,double)));

    //manejo de errores
    connect(SearchByTitleQuery,SIGNAL(Error(int,QString,QString)),this,SIGNAL(Error(int,QString,QString)));
    connect(TextCompletionQuery,SIGNAL(Error(int,QString,QString)),this,SIGNAL(Error(int,QString,QString)));



}



SearchEngine::~SearchEngine()
{
    if(SearchByTitleQuery != 0 )
    {
        SearchByTitleQuery->terminate();
#ifndef linux
        SearchByTitleQuery->wait();
#endif
        delete SearchByTitleQuery;
    }

    if(TextCompletionQuery !=0){
        TextCompletionQuery->terminate();
#ifndef linux
        TextCompletionQuery->wait();
#endif
        delete TextCompletionQuery;
    }

}


void SearchEngine::stop(){

    SearchByTitleQuery->terminate();
    SearchByTitleQuery->wait();

    TextCompletionQuery->terminate();
    TextCompletionQuery->wait();

    delete SearchByTitleQuery;
    delete TextCompletionQuery;

    SearchByTitleQuery =0;
    TextCompletionQuery =0;

}

void SearchEngine::UseMultiFieldSearch()
{
    this->MultiFieldSearch=true;
    CLuceneHelper * lucene = CLuceneHelper::getInstance();
    lucene->UseMultiFieldSearch();
}

void SearchEngine::SetSearchField(const QString Field)
{
    this->SearchField=Field;
    CLuceneHelper * lucene = CLuceneHelper::getInstance();
    lucene->SetSearchField(Field);
}


void SearchEngine::check(){

    if (SearchByTitleQuery == 0){
        SearchByTitleQuery = new PQuery("wikidb");

    }

    if (TextCompletionQuery == 0){
        TextCompletionQuery = new PQuery("wikidbindex");
    }


}

void SearchEngine::SearchByTitle(const QString &str,bool useMatch){

    /*if(AppConfig::instance()->IsSourceDocAvailable()){
        emit Error(3,"No ha sido posible abrir la base de datos.","");
        QList<QPair<QString, int> > results;
        emit SearchFinished(0);
        emit OnSearchByTitleReady(results,0);
        return;
    }*/

    //QMutex mutex;
    //mutex.lock();
    SearchTitle = str;
    emit SearchStarted();
    //buscar=Cuba&useMatch=T
    //http://localhost:8080/Lucene?buscar=cuba
    //&MultiFieldSearch=T&SearchField=Título
    if(AppConfig::instance()->IsUsingRemoteServer()){
        if(useMatch){
            this->useMatch = true;
            QString UrlString = AppConfig::instance()->getServerURL();
            UrlString = UrlString + QString("Lucene?buscar=%1").arg(str);
            if(this->MultiFieldSearch){
                UrlString.append("&MultiFieldSearch=T");
                this->MultiFieldSearch=false;
            }

            if(this->SearchField!="Título"){
                UrlString.append(QString("&SearchField=%1").arg(this->SearchField));
                this->SearchField="Título";
            }


            //qDebug()<<UrlString;

            QUrl ServerUrl(UrlString);
            this->ManagerSearchByTitle->get(QNetworkRequest(ServerUrl));
            //qDebug() << ServerUrl;

        } else {
            this->useMatch = false;
            QString UrlString = AppConfig::instance()->getServerURL();
            UrlString = UrlString + QString("index/%1").arg(str);
            QUrl ServerUrl(UrlString);
            this->ManagerSearchByTitle->get(QNetworkRequest(ServerUrl));
        }
    } else {
        if(useMatch){
            this->useMatch = true;
            CLuceneHelper * lucene = CLuceneHelper::getInstance();
            if(lucene->isAvailable()){
                connect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinish(QList<QPair<QString,int> >,double)));
                QtConcurrent::run(lucene,&CLuceneHelper::Search,str,0);
            }
            else {
                QList<QPair<QString, int> > results;
                emit SearchFinished(0);
                disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinish(QList<QPair<QString,int> >,double)));
                emit OnSearchByTitleReady(results,0);
            }

        } else {
            this->useMatch = false;
            QString query =
                    "SELECT * FROM "
                    " (SELECT content,old_id,old_text FROM"
                    " wikitext JOIN searchindex ON old_id=docid"
                    " WHERE content MATCH '"+str+"')"
                    " WHERE content LIKE '"+str+"' LIMIT 1";

            SearchByTitleQuery->execute(query);
        }
    }
    //mutex.unlock();
}

QString SearchEngine::getSearchTitle() const{
    return SearchTitle;
}

void SearchEngine::SearchById(const int &id){

    SearchTitle = "";
    /*
    if(AppConfig::instance()->IsSourceDocAvailable()){
        emit Error(3,"No ha sido posible abrir la base de datos.","");
        QList<QPair<QString, int> > results;
        emit SearchFinished(0);
        emit OnSearchByTitleReady(results,0);
        return;
    }*/


    emit SearchStarted();
    /*
    if(AppConfig::instance()->IsUsingRemoteServer()){
        //http://100.0.0.10:8080/index/?id=500

        QString UrlString = AppConfig::instance()->getServerURL();
        UrlString = UrlString + QString("index/?id=%1").arg(id);
        QUrl ServerUrl(UrlString);
        this->ManagerSearchByTitle->get(QNetworkRequest(ServerUrl));
        qDebug() << ServerUrl;
    } else {
        //QString idstr;
        //idstr.setNum(id);
        QString query = QString(" SELECT content,old_id,old_text FROM wikitext JOIN searchindex ON old_id=docid where docid = '%1' LIMIT 1").arg(id);
        SearchByTitleQuery->execute(query);
    }
*/
    QString query = QString(" SELECT content,old_id,old_text FROM wikitext JOIN searchindex ON old_id=docid where docid = '%1' LIMIT 1").arg(id);
    SearchByTitleQuery->execute(query);
}

void SearchEngine::TextCompletion(const QString &str,bool useLucene){
    SearchTitle="";
    emit SearchStarted();
    //http://localhost:8080/Lucene?buscar=cuba
    if(AppConfig::instance()->IsUsingRemoteServer()){

        QString UrlString = AppConfig::instance()->getServerURL();
        UrlString = UrlString + QString("Lucene?buscar=%1").arg(str);
        QUrl ServerUrl(UrlString);
        this->ManagerTextCompletion->get(QNetworkRequest(ServerUrl));
        qDebug() << ServerUrl;
    } else {
        CLuceneHelper * lucene = CLuceneHelper::getInstance();
        if(lucene->isAvailable()){
            connect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinishTextCompletion(QList<QPair<QString,int> >,double)));
            QtConcurrent::run(lucene,&CLuceneHelper::Search,str,0);
        } else {
            QList<QPair<QString, int> > results;
            emit SearchFinished(0);
            disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinishTextCompletion(QList<QPair<QString,int> >,double)));
            emit OnTextCompletionReady(results,false,0);
        }

    }


}

void SearchEngine::slotSearchByTitle( const QList<QSqlRecord>& records,double tiempo )
{

    emit SearchFinished(records.size());
    emit OnSearchByTitleReady(records,useMatch,tiempo);
}


void SearchEngine::slotTextCompletion( const QList<QSqlRecord>& records ,double tiempo)
{
    emit SearchFinished(records.size(),true);
    emit OnTextCompletionReady(records,tiempo);
}


void SearchEngine::OnCluceneSearchFinish(const QList<QPair<QString, int> >& results,double tiempo){
    emit SearchFinished(results.size());
    disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinish(QList<QPair<QString,int> >,double)));
    emit OnSearchByTitleReady(results,tiempo);
}

void SearchEngine::OnCluceneSearchFinishTextCompletion(const QList<QPair<QString, int> >& results,double tiempo){
    emit SearchFinished(results.size());
    disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinishTextCompletion(QList<QPair<QString,int> >,double)));
    emit OnTextCompletionReady(results,CLuceneHelper::getInstance()->isAvailable(),tiempo);
}


void SearchEngine::SearchByTitleReplyFinished( QNetworkReply * reply ) {

    QList<QPair<QString, int> > results;

    if(reply->error()){
        emit SearchFinished(0);
        emit OnSearchByTitleReady(results,0);
        emit Error(4,"Error de conexión.",reply->errorString());
    } else {
        QByteArray jsonString = reply->readAll();
        QJson::Parser parser;
        bool ok;
        QVariant variant = parser.parse(jsonString,&ok);
        LuceneDocument doc;


        QPair<QString, int> item;
        double tiempo=0;

        if(ok){
            QJson::QObjectHelper::qvariant2qobject(variant.toMap(), &doc);

            for (int i = 0; i < doc.Arts().size(); i++) {
                QVariantMap data = doc.Arts().at(i).toMap();

                item.first = data.value("text").toString();
                item.second = data.value("id").toInt();
                results.append(item);
                //qDebug() << data.value("id").toInt();
                //qDebug() << data.value("text").toString();
                //break;

            }
        }else{
            emit Error(5,"Los datos provenientes del servidor contienen un formato inválido.","");
        }

        emit SearchFinished(results.size());
        emit OnSearchByTitleReady(results,tiempo);

    }

    //qDebug()<<"OnSearchByTitleReady";
    //qDebug()<<reply->readAll();


    reply->deleteLater();
}

void SearchEngine::TextCompletionReplyFinished(QNetworkReply *reply)
{

    QList<QPair<QString, int> > results;

    if(reply->error()){
        /*
                NoError = 0,
                // network layer errors [relating to the destination server] (1-99):
                ConnectionRefusedError = 1,
                RemoteHostClosedError,
                HostNotFoundError,
                TimeoutError,
                OperationCanceledError,
                SslHandshakeFailedError,
                TemporaryNetworkFailureError,
                UnknownNetworkError = 99,

                // proxy errors (101-199):
                ProxyConnectionRefusedError = 101,
                ProxyConnectionClosedError,
                ProxyNotFoundError,
                ProxyTimeoutError,
                ProxyAuthenticationRequiredError,
                UnknownProxyError = 199,

                // content errors (201-299):
                ContentAccessDenied = 201,
                ContentOperationNotPermittedError,
                ContentNotFoundError,
                AuthenticationRequiredError,
                ContentReSendError,
                UnknownContentError = 299,

                // protocol errors
                ProtocolUnknownError = 301,
                ProtocolInvalidOperationError,
                ProtocolFailure = 399
                */
        emit SearchFinished(0);
        emit OnTextCompletionReady(results,true,0);
        emit Error(4,"Error de conexión.",reply->errorString());
    } else {
        QByteArray jsonString = reply->readAll();
        QJson::Parser parser;
        bool ok;
        QVariant variant = parser.parse(jsonString,&ok);
        LuceneDocument doc;


        QPair<QString, int> item;
        double tiempo=0;

        if(ok){
            QJson::QObjectHelper::qvariant2qobject(variant.toMap(), &doc);

            for (int i = 0; i < doc.Arts().size(); i++) {
                QVariantMap data = doc.Arts().at(i).toMap();

                item.first = data.value("text").toString();
                item.second = data.value("id").toInt();
                results.append(item);
                //qDebug() << data.value("id").toInt();
                //qDebug() << data.value("text").toString();
                //break;

            }
        }else{
            emit Error(5,"Los datos provenientes del servidor contienen un formato inválido.","");
        }

        emit SearchFinished(results.size());
        emit OnTextCompletionReady(results,true,tiempo);
    }

    qDebug()<<"TextCompletionReplyFinished";
    //qDebug()<<reply->readAll();


    reply->deleteLater();
}

//void SearchEngine::connectNotify(const char *signal)
//{
//    qDebug()<<"Coneccted "<< signal;
//}


