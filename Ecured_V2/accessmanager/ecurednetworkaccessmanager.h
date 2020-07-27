#ifndef ECUREDNETWORKACCESSMANAGER_H
#define ECUREDNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QWebView>
#include <QWebFrame>
#include <QNetworkReply>
#include <searchengine.h>
#include <wikihelpeer.h>


class EcuredNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    EcuredNetworkAccessManager(QObject *parent);
    ~EcuredNetworkAccessManager();
    virtual QNetworkReply * get( const QNetworkRequest & request );
    virtual QNetworkReply * post ( const QNetworkRequest & request, QIODevice * data );



private:
    QString url;



private slots:
    void openExternalUrl();
    void closeRequest(QNetworkReply *reply);

protected:
    virtual QNetworkReply *createRequest(Operation op,
                                         const QNetworkRequest &request, QIODevice *outgoingData = 0);
    QString mimeFromUrl(const QUrl &url);





};

class EcuredNetworkReply : public QNetworkReply
{
    Q_OBJECT
public:
    EcuredNetworkReply(const QNetworkRequest &request, const QByteArray &fileData,const QString &mimeType);
    EcuredNetworkReply(const QNetworkRequest &request);
    ~EcuredNetworkReply();

    virtual void abort();

    virtual qint64 bytesAvailable() const
    { return data.length() + QNetworkReply::bytesAvailable(); }
    virtual bool isSequential() const { return true; }

protected:
    virtual qint64 readData(char *data, qint64 maxlen);

private:
    QByteArray data;
    qint64 origLen;

    QWebView *view ;
    QWebFrame *frame;
    QString WikiParser,WikiMarkupParser;
    QString TextContainer;
    QString search;

    WikiHelper *wikihelper;


private slots:
    void OnSearchByTitleReady(const QList<QSqlRecord>& records ,bool useMatch,double tiempo);
    void OnSearchByTitleReady(const QList<QPair<QString, int> >& results,double tiempo);
    void on_frame_loadFinished(bool);
    void populateJavaScriptWindowObject();
//    void test();


};

#endif // ECUREDNETWORKACCESSMANAGER_H
