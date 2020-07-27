/**
  @file
  @author Stefan Frings
*/

#ifndef LUCENECONTROLLER_H
#define LUCENECONTROLLER_H

#include <QList>
#include <QPair>

#include <CLucene.h>
#include <CLucene/StdHeader.h>
#include <CLucene/util/Misc.h>
#include <CLucene/snowball/SnowballAnalyzer.h>
#include <QStringList>

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::queryParser;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::search;

class LuceneController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(LuceneController);
public:

    /** Constructor */
    LuceneController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
private:
    snowball::SnowballAnalyzer an;
    bool MultiFieldSearch;
    QString DirIndex;
    QString SearchField;
    QString LastQueryString;
    QString LastErrorString;


    bool isAvailable();
    void Search(HttpRequest& request, HttpResponse& response,const QString &cad,size_t limite=0);
    void OnSearchFinish(HttpRequest& request, HttpResponse& response,const QList<QPair<QString, int> >& results,double tiempo);
    void ProcessError(HttpRequest& request, HttpResponse& response,const int ErrType,const QString Msg,const QString OrgMsg);
    QString toError(const int number) const;
    QStringList getFieldNames();
    QString LastQuery() const;

};

#endif // LUCENECONTROLLER_H
