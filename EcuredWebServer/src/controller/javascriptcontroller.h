/**
  @file
  @author Stefan Frings
*/

#ifndef JAVASCRIPTCONTROLLER_H
#define JAVASCRIPTCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QScriptEngine>
/**
  This controller generates a website using the template engine.
  It generates a Latin1 (ISO-8859-1) encoded website from a UTF-8 encoded template file.
*/

class JavascriptController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(JavascriptController);
public:

    /** Constructor */
    JavascriptController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
private:
    QScriptEngine myEngine;
};

#endif // JAVASCRIPTCONTROLLER_H
