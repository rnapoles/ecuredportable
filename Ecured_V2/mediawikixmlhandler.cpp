#include "mediawikixmlhandler.h"
#include <QXmlParseException>

QString strip(const QString& str) {
  int n = str.size() - 1;
  QString r;
  for (; n >= 0; --n) {
    if (!str.at(n).isSpace()) {
        r.append(str.at(n));
    }
  }
  return r;
}


MediawikiXmlHandler::MediawikiXmlHandler(const QString &db)
{
    IsTitle = false;
    IsText = false;
    initErr =false;
    dbm = new DatabaseManager(db);

    dbm->DeleteDB();
    if(dbm->OpenDB()){
        if(!dbm->CreateDb()){
            QXmlParseException e(dbm->LastErrorStr(), 0, 0, 0, 0);
            fatalError(e);
            initErr = true;
        }
    }
}

MediawikiXmlHandler::~MediawikiXmlHandler()
{
    dbm->CloseDb();
    delete dbm;
}

bool MediawikiXmlHandler::startElement(const QString &namespaceURI, const QString &localName,const QString &qName, const QXmlAttributes &attributes){

    if(initErr){
        QXmlParseException e("Error de inicio", 0, 0, 0, 0);
        fatalError(e);
        return false;
    }

    if(qName.compare("title")==0){
        Title = "";
        IsTitle = true;
    }

    if(qName.compare("text")==0){
        Text.clear();
        //Texts.clear();
        IsText = true;
    }

    return true;
}

bool MediawikiXmlHandler::endElement(const QString &namespaceURI, const QString &localName,const QString &qName){
    if(qName.compare("title")==0){
        IsTitle = false;
    }


    if(qName.compare("text")==0){
        IsText = false;
    }

    static int c =0,b=0;
    if((qName.compare("page")==0) && (!Title.isEmpty()))
        if(!strip(Text).isEmpty()){
        if(dbm->IsOpen()){

            if(!dbm->Insert(Title,Text)){
                QXmlParseException e(dbm->LastErrorStr(), 0, 0, 0, 0);
                fatalError(e);
                return false;
                c++;
            } else {
                emit ReportMsg(Text);
                b++;
            }
        }
    }

    if(qName.compare("mediawiki")==0){

        if(!dbm->SetDbInfo(true,true,b)){
            QXmlParseException e(dbm->LastErrorStr(), 0, 0, 0, 0);
            fatalError(e);
        }

    }


    return true;
}

bool MediawikiXmlHandler::characters(const QString &str){

    if(IsTitle){
        this->Title = str;
    }


    if(IsText){
        this->Text = str;
    }


    return true;
}

bool MediawikiXmlHandler::fatalError(const QXmlParseException &exception){
    QString msg = QString("Error en la línea: %1\n%2").arg(exception.lineNumber()).arg(exception.message());
    emit Error(msg);
    return false;
}


QString MediawikiXmlHandler::errorString() const{
    return QString();
}

