/**
  @file
  @author Stefan Frings
*/

#include "requestmapper.h"
#include "static.h"
#include "controller/staticfilecontroller.h"
#include "controller/dumpcontroller.h"
#include "controller/templatecontroller.h"
#include "controller/formcontroller.h"
#include "controller/fileuploadcontroller.h"
#include "controller/sessioncontroller.h"
#include "controller/indexcontroller.h"
#include "controller/lucenecontroller.h"

RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent) {}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {

    QByteArray path=request.getPath();
    QByteArray root("/");

    qDebug("RequestMapper: path=%s",path.data());

    if (path==root){
        IndexController().redirect(request, response);
    }

    else if (path=="/index"){
        IndexController().redirect(request, response);
    }

    else if (path=="/Lucene"){
        LuceneController().service(request, response);
    }

    else if (path.startsWith("/dump")) {
        DumpController().service(request, response);
    }

    else if (path.startsWith("/template")) {
        TemplateController().service(request, response);
    }

    else if (path.startsWith("/form")) {
        FormController().service(request, response);
    }

    else if (path.startsWith("/file")) {
        FileUploadController().service(request, response);
    }

    else if (path.startsWith("/session")) {
        SessionController().service(request, response);
    }

    else if (path.startsWith("/index")) {
        IndexController().service(request, response);
    }

    // All other pathes are mapped to the static file controller.
    else {
        Static::staticFileController->service(request, response);
    }
}
