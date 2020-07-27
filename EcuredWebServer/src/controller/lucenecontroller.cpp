/**
  @file
  @author Stefan Frings
*/

#include "lucenecontroller.h"
#include <CLucene/queryParser/MultiFieldQueryParser.h>
#include "CommonDefines.h"
#include "parser.h"
#include "serializer.h"
#include "qobjecthelper.h"
#include "lucenedocument.h"
#include <LuceneTextManipulation.h>



LuceneController::LuceneController():HttpRequestHandler(0),an(_T("Spanish")) {
    this->DirIndex = "s_index";
    this->SearchField = "Título";
    //this->SearchField = "title";
    this->MultiFieldSearch = false;

}

void LuceneController::service(HttpRequest& request, HttpResponse& response) {
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    QString path(request.getPath());
    QMultiMap<QByteArray,QByteArray> ParameterMap = request.getParameterMap();
    if (ParameterMap.count() > 0){
        if(ParameterMap.contains("buscar")){
            //useMatch
            //buscar="+text+"&useMatch=T&MultiFieldSearch=T&SearchField=Título"
            QString buscar = ParameterMap.value("buscar");
            
            this->SearchField = "Título";
            if(ParameterMap.contains("SearchField")){
				this->SearchField= ParameterMap.value("SearchField");
            }
            
            //response.write(buscar.toUtf8());
            if(ParameterMap.contains("MultiFieldSearch")){
				this->MultiFieldSearch = true;
            } else {
				this->MultiFieldSearch = false;
            }
            this->Search(request,response,buscar);
            
        }
    }

}

void LuceneController::Search(HttpRequest& request, HttpResponse& response,const QString &cad, size_t limite)
{
    QList<QPair<QString, int> > records;


    if(this->isAvailable()){;

        QPair<QString, int> item;

        try {

            IndexReader * reader = IndexReader::open(DirIndex.toLatin1().constData(),false);
            IndexSearcher s(reader);

            Query* q;
            TCHAR *QueryString =  QStringToTChar(cad);

            if(this->getFieldNames().count()==0) {
                this->ProcessError(request,response,ErrLucene,this->LastErrorString,"");
            }

            if(!this->MultiFieldSearch || (this->getFieldNames().count()<2)){
                QRegExp rx1("(Título|Contenido)(:)");
                TCHAR *TCharSearchField =  QStringToTChar(SearchField);

                q = QueryParser::parse(QueryString,TCharSearchField,&an);

                if(!cad.contains(rx1)){
                    q = QueryParser::parse(QueryParser::escape(QueryString),TCharSearchField,&an);
                } else {
                    q = QueryParser::parse(QueryString,TCharSearchField,&an);
                }

                //SearchField = "Título";
                delete [] TCharSearchField;
            } else {

                const TCHAR* fields[] = {_T("Título"), _T("Contenido"), NULL };
                MultiFieldQueryParser mfqp(fields, &an);

                q = mfqp.parse(QueryString);
                MultiFieldSearch = false;
            }

            LastQueryString = TCharToQString(q->toString());
            delete [] QueryString;


            clock_t t1,t2;
            t1 = clock();
            Hits* h = s.search(q);

            size_t cant_doc =h->length();

            QString ss;
            ss.setNum(cant_doc);
            //response.write(ss.toUtf8());
            //response.write(LastQueryString.toLatin1());
            //qDebug()<<SearchField;
            //qDebug()<<LastQueryString;

            if(limite != 0)
                if(cant_doc > limite) cant_doc = limite;

            for ( size_t i=0;i<cant_doc;i++ ){
                Document* doc = &h->doc(i);
                //QString title = TCharToQString(doc->get(_T("title")));
                QString title = TCharToQString(doc->get(_T("Título")));
                QString id = TCharToQString(doc->get(_T("id")));

                item.first = title;
                item.second = id.toInt();
                //qDebug()<<title;
                //qDebug()<<id;
                records.append(item);
            }
            t2 = clock();
            double tiempo = double( t2 - t1 ) / (double)CLOCKS_PER_SEC;

            _CLLDELETE(h);
            _CLLDELETE(q);

            s.close();
            reader->close();
            _CLLDELETE(reader);

            this->OnSearchFinish(request,response,records,tiempo);

        } catch (CLuceneError err) {
            QString StrExcept =  QString(err.what()).toUtf8();
            this->ProcessError(request,response,ErrLucene,toError(err.number()),StrExcept);
        }

    } else {
        this->ProcessError(request,response,ErrLucene,this->LastErrorString,"");
    }
}

void LuceneController::OnSearchFinish(HttpRequest& request, HttpResponse& response,const QList<QPair<QString, int> > &results, double tiempo)
{
    QVariantList listado;
    LuceneDocument doc;
    //QString output;
    //output.append("{\"data\":[");
    //response.write();
    int size = results.size();
    for (int i = 0; i < size ; ++i) {
       QVariantMap data;
       data.insert("text",results.at(i).first);
       data.insert("id",results.at(i).second);
       listado << data;
    }
    QJson::Serializer serializer;
    doc.setArts(listado);

    QVariantMap json = QJson::QObjectHelper::qobject2qvariant(&doc);
    response.write(serializer.serialize(json));
}

void LuceneController::ProcessError(HttpRequest &request, HttpResponse &response,const int ErrType, const QString Msg, const QString OrgMsg)
{
    LuceneDocument doc;
    QJson::Serializer serializer;

    doc.setError(true);
    doc.setErrorText(Msg);

    QVariantMap json = QJson::QObjectHelper::qobject2qvariant(&doc);
    response.write(serializer.serialize(json));

}

QString LuceneController::toError(const int number) const
{
    switch (number) {

    case  CL_ERR_IO :
        return QString("Error de entrada y salida.                                                                ");
        break;
    case  CL_ERR_NullPointer :
        return QString("Puntero nulo.                                                                             ");
        break;
    case  CL_ERR_Runtime :
        return QString("Error en tiempo de ejecución.                                                             ");
        break;
    case  CL_ERR_IllegalArgument :
        return QString("Argumento iválido.                                                                        ");
        break;
    case  CL_ERR_Parse :
        return QString("Consulta inválida.\nConsulte el manual de ayuda para las diferentes opciones de búsqueda");
        //return QString("Consulta inválida.                                                                        ");
        break;
    case  CL_ERR_TokenMgr :
        return QString("Error al reservar memoria para el token.                                               ");
        break;
    case  CL_ERR_UnsupportedOperation :
        return QString("Operación no soportada.                                                               ");
        break;
    case  CL_ERR_InvalidState :
        return QString("intenta acceder a un documento eliminado.                                             ");
        break;
    case  CL_ERR_IndexOutOfBounds :
        return QString("El índice de la matriz se encuentra fuera de los límites.                             ");
        break;
    case  CL_ERR_TooManyClauses :
        return QString("Error desconocido.                                                                    ");
        break;
    case  CL_ERR_RAMTransaction :
        return QString("Error de transacción en memoria.                                                      ");
        break;
    case  CL_ERR_InvalidCast :
        return QString("Error de conversión.                                                                  ");
        break;
    case  CL_ERR_IllegalState:    // Sub-error: AlreadyClosed
        return QString("Estado desconocido.                                                                   ");
        break;
    case  CL_ERR_UnknownOperator :
        return QString("Operador desconocido.                                                                 ");
        break;
    case  CL_ERR_ConcurrentModification :
        return QString("No es un número de hit válido.                                                        ");
        break;
    case  CL_ERR_CorruptIndex :
        return QString("El índice está corrupto.                                                              ");
        break;
    case  CL_ERR_NumberFormat :
        return QString("Error al formatear la cadena.                                                         ");
        break;
    case  CL_ERR_AlreadyClosed :
        return QString("El índice esta cerrado.                                                               ");
        break;
    case  CL_ERR_StaleReader :
        return QString("El índice está desactualizado  y ya no es válido para las operaciones de eliminación,\nrecuperación o la normalización.");
        break;
    case  CL_ERR_LockObtainFailed :
        return QString("Índice bloqueado para escritura.                                                       ");
        break;
    case  CL_ERR_Merge : //< Exception thrown if there are any problems while executing a merge.
        return QString("Error de mezcla de segmentos.                                                          ");
        break;
    case  CL_ERR_MergeAborted :
        return QString("Mezcla abortada.                                                                           ");
        break;
    case  CL_ERR_OutOfMemory :
        return QString("Fuera de memoria.                                                                          ");
        break;
    case  CL_ERR_FieldReader :
        return QString("Error al leer campo.                                                                       ");
        break;
    default:
        return QString("Error desconocido.                                                                         ");

    }
}

QStringList LuceneController::getFieldNames()
{
    try {

        QStringList FieldNames;
        StringArrayWithDeletor fn1;
        IndexReader * reader = IndexReader::open(DirIndex.toLatin1().constData(),false);
        reader->getFieldNames(IndexReader::INDEXED , fn1);
        reader->close();
        _CLLDELETE(reader);

        int fn1count = fn1.size();

        for (int n=0;n<fn1count;n++ ){
            QString flieldname = TCharToQString(fn1[n]);
            if(flieldname.compare("id")==0) continue;
            FieldNames.append(flieldname);
        }
        fn1.clear();

        return FieldNames;

    } catch (CLuceneError err) {
        //this->ProcessError(request,response,ErrLucene,toError(err.number()),QString(err.what()));
        this->LastErrorString = toError(err.number());
    }
    return QStringList();
}

QString LuceneController::LastQuery() const
{
    return LastQueryString;
}

bool LuceneController::isAvailable()
{
    bool valid = true;

    if(!QFile::exists(DirIndex)){
        this->LastErrorString = QString("El índice de búsqueda %1 no existe o está dañado").arg(DirIndex);
        //this->ProcessError(request,response,ErrFile,ErrStr,"");
        return false;
    }


    if(!IndexReader::indexExists(DirIndex.toLatin1().constData())){
        this->LastErrorString = QString("El indice %1 no existe").arg(DirIndex);
        //this->ProcessError(request,response,ErrFile,ErrStr,"");
        return false;
    }

    return valid;
}

