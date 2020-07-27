/**
  @file
  @author Stefan Frings
*/

#ifndef INDEXCONTROLLER_H
#define INDEXCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QSqlRecord>
#include <QSqlError>

/**
  This controller generates a website using the template engine.
  It generates a Latin1 (ISO-8859-1) encoded website from a UTF-8 encoded template file.
*/

class IndexController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(IndexController);
public:

    /** Constructor */
    IndexController(QObject*parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    void redirect(HttpRequest& request, HttpResponse& response);

    QString mimeFromUrl(const QString &path)        ;

private:
    QString search;
    void SearchByTitle(HttpRequest& request, HttpResponse& response,const QString & str,bool useMatch=false);//
    void SearchById(HttpRequest& request, HttpResponse& response,const QString &id);
    void RunQuery(const QString& query,HttpRequest& request, HttpResponse& response);
    void ThrowError(HttpRequest& request, HttpResponse& response,const QString Msg,const QSqlError err);

signals:

    void results(HttpResponse& response, const QList<QSqlRecord>& records ,double tiempo);
    void Error(HttpResponse& response,const int ErrType,const QString Msg,const QString OrgMsg);

private slots:
    void ProcessResults(HttpRequest& request,HttpResponse& response, const QList<QSqlRecord>& records ,double tiempo);

};



#endif // INDEXCONTROLLER_H
