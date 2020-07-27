#include "qurlhelper.h"

QUrlHelper::QUrlHelper(const QString &url):QUrl(url)
{

}

QString QUrlHelper::obtenerParametro() const {

    if(host().isEmpty()){
        QString thepath = path();
       int lastindex = thepath.lastIndexOf("/");
       return thepath.mid(lastindex+1);
   }

    return "";
}
