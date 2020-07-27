/**
  @file
  @author Stefan Frings
*/

#include "javascriptcontroller.h"
#include "../static.h"

JavascriptController::JavascriptController(){}

void JavascriptController::service(HttpRequest& request, HttpResponse& response) {

    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    QScriptValue scriptResponse = myEngine.newQObject(&response);
    myEngine.globalObject().setProperty("response", scriptResponse);

    QScriptValue result = myEngine.evaluate("response.write(\"Esto es un pb desde javascript<br>\",false,true);response.write(1+1,false,true);");
    if (myEngine.hasUncaughtException()) {
        int line = myEngine.uncaughtExceptionLineNumber();
        QString ErrorStr("uncaught exception at line %1 %2");
        ErrorStr = ErrorStr.arg(line).arg(result.toString());
        response.write(ErrorStr.toUtf8(),true);
    }

}
