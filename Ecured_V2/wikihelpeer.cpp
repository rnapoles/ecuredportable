#include "wikihelpeer.h"

WikiHelper::WikiHelper(QObject *parent) :
    QObject(parent)
{

}

WikiHelper::~WikiHelper()
{

}

void WikiHelper::setTextContainer(const QString &cad){
   TextContainer = cad;
}

QString WikiHelper::getTextContainer(){
    return TextContainer;
}

QString WikiHelper::convertToBase64(const QString &str){
    QByteArray text(str.toLatin1());
    text = text.toBase64();
    return QString(text);
}
/*
void WikiHelper::debug(const QString &str){
}
*/
