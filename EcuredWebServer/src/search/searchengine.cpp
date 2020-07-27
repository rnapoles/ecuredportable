#include "searchengine.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlRecord>
#include <QHashIterator>
#include <QHash>
#include "clucenehelper.h"
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


void SearchEngine::check(){

    if (SearchByTitleQuery == 0){
        SearchByTitleQuery = new PQuery("wikidb");

    }

    if (TextCompletionQuery == 0){
        TextCompletionQuery = new PQuery("wikidbindex");
    }


}

void SearchEngine::SearchByTitle(const QString &str,bool useMatch){

    //QMutex mutex;
    //mutex.lock();
    SearchTitle = str;
    emit SearchStarted();

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

        }
    else {
        this->useMatch = false;
        QString query =
                "SELECT * FROM "
                " (SELECT content,old_id,old_text FROM"
                " wikitext JOIN searchindex ON old_id=docid"
                " WHERE content MATCH '"+str+"')"
                " WHERE content LIKE '"+str+"' LIMIT 1";

        SearchByTitleQuery->execute(query);
    }
    //mutex.unlock();
}

QString SearchEngine::getSearchTitle() const{
    return SearchTitle;
}

void SearchEngine::SearchById(const int &id){

    SearchTitle = "";
    emit SearchStarted();
    QString idstr;
    idstr.setNum(id);
    QString query = " SELECT content,old_id,old_text FROM wikitext JOIN searchindex ON old_id=docid where docid = '"+idstr+"' LIMIT 1";     
    SearchByTitleQuery->execute(query);
}

void SearchEngine::TextCompletion(const QString &str,bool useLucene){
    SearchTitle="";
    emit SearchStarted();
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
    disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinish(QList<QPair<QString,int> >,double)));
    emit OnSearchByTitleReady(results,tiempo);
}

void SearchEngine::OnCluceneSearchFinishTextCompletion(const QList<QPair<QString, int> >& results,double tiempo){
    disconnect(CLuceneHelper::getInstance(),SIGNAL(OnSearchFinish(QList<QPair<QString,int> >,double)),this,SLOT(OnCluceneSearchFinishTextCompletion(QList<QPair<QString,int> >,double)));
    emit OnTextCompletionReady(results,CLuceneHelper::getInstance()->isAvailable(),tiempo);
}



