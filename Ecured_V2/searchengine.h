#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H
#include <QSqlDatabase>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrentRun>
#include "querythread.h"
//#include "dbquery.h"


#include <QNetworkAccessManager>
#include <QNetworkReply>


#ifndef RENDERTHREAD_H
    #define PQuery DBQuery
#else
    #define PQuery QueryThread
#endif

class MainWindow;

class SearchEngine: public QObject
{

Q_OBJECT

public:

    void SearchByTitle(const QString & str,bool useMatch=false);//
    void SearchById(const int &id);
    void TextCompletion(const QString & str,bool useLucene=true);
    QString getSearchTitle() const;
    void stop();

    void UseMultiFieldSearch(void);
    void SetSearchField (const QString Field);

    static SearchEngine *instance();
    ~SearchEngine();

signals:
    void SearchStarted();
    void SearchFinished(int size,bool isTextCompletion=false);
    void OnTextCompletionReady(const QList<QSqlRecord>& records,double tiempo);
    void OnTextCompletionReady(const QList<QPair<QString, int> >& results,bool lucene,double tiempo);    
    void OnSearchByTitleReady(const QList<QSqlRecord>& records,bool useMatch,double tiempo);
    void OnSearchByTitleReady(const QList<QPair<QString, int> >& results,double tiempo);
    void Error(const int ErrType,const QString Msg,const QString OrgMsg);

private:
    explicit SearchEngine( QObject* parent = 0);
    void check();
    static SearchEngine *s_instance;
    PQuery* SearchByTitleQuery;
    PQuery  *TextCompletionQuery;
    QString SearchTitle;
    bool useMatch;

    bool MultiFieldSearch;
    QString SearchField;

    //QMutex Mutex;
    //MainWindow *mainwindow;

    QNetworkAccessManager *manager;
    QNetworkAccessManager *ManagerSearchByTitle;
    QNetworkAccessManager *ManagerTextCompletion;

private slots:
  void slotSearchByTitle( const QList<QSqlRecord>& records ,double tiempo);
  void slotTextCompletion( const QList<QSqlRecord>& records ,double tiempo);

  void OnCluceneSearchFinish(const QList<QPair<QString, int> >& results,double tiempo);
  void OnCluceneSearchFinishTextCompletion(const QList<QPair<QString, int> >& results,double tiempo);

  void SearchByTitleReplyFinished(QNetworkReply *reply);
  void TextCompletionReplyFinished(QNetworkReply *reply);

protected:
    //virtual void connectNotify ( const char * signal );
};

#endif // SEARCHENGINE_H
