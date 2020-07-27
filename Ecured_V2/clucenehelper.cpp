#include "clucenehelper.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QProgressDialog>
//#include <QMutex>
#include <QXmlStreamReader>
#include <QSqlError>
#include "util.h"
#include <databasemanager.h>
#include <QFile>
#include <CLucene/queryParser/MultiFieldQueryParser.h>
#include <QDebug>

//CL_NS_USE2(analysis, snowball);

#define IndexContent
#define IndexContentDebug1

TCHAR* QStringToTChar(const QString &str)
{
    TCHAR *string = new TCHAR[(str.length() +1) * sizeof(TCHAR)];
    memset(string, 0, (str.length() +1) * sizeof(TCHAR));
    str.toWCharArray(string);
    return string;
}

QString TCharToQString(const TCHAR *string)
{
    QString retValue = QString::fromWCharArray(string);
    return retValue;
}

CLuceneHelper *CLuceneHelper::s_instance = 0;

CLuceneHelper::CLuceneHelper(QObject *parent) :
    QObject(parent),an(_T("Spanish"))
{
    qRegisterMetaType< QList<QPair<QString, int> > >( "QList<QPair<QString, int> >" );

    SearchField = "Título";
    MultiFieldSearch = false;

}


CLuceneHelper::~CLuceneHelper(){
    _lucene_shutdown();
}

void CLuceneHelper::indexContent(bool indexContentField){
    //QMutex mutex;
    //mutex.lock();
    IndexWriter *writer = NULL;

    try {

        writer = _CLNEW IndexWriter(DirIndex.toLatin1().constData(),&an,true);
        writer->setMaxFieldLength(0x7FFFFFFFL);
        writer->setUseCompoundFile(false);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","info_s");
            db.setDatabaseName(this->DataBase);

            if(db.open()){

                int start = 0;
                int size = 100;
                int contador = 0;

                int total = -1;
                DatabaseInfo dbinfo;
                DatabaseManager dbm(this->DataBase);
                if(dbm.OpenDB()){
                    if(dbm.GetDbInfo(dbinfo)){
                        total = dbinfo.NumArts();
                    }
                    dbm.CloseDb();
                } else {
                    emit indexContentStarted(0);
                    emit Error(ErrBd,"No se pudo consultar la fuente documental",dbm.LastErrorStr());
                    emit indexContentFinished();
                }

                emit indexContentStarted(total);
                QString strQuery;


                while (contador < total ){

                    if(indexContentField == true){
                        strQuery = QString("SELECT docid,content,old_text FROM  wikitext JOIN searchindex ON old_id=docid LIMIT %1,%2;")
                                .arg(start).arg(size);
                    } else {
                        strQuery = QString("SELECT docid,content FROM searchindex LIMIT %1,%2;")
                                .arg(start).arg(size);
                    }


                    QSqlQuery query(db);

                    if(!query.exec(strQuery)){
                        emit indexContentStarted(0);
                        emit Error(ErrBd,"No se pudo consultar la fuente documental",dbm.LastErrorStr());
                        emit indexContentFinished();
                        return;
                    }

                    while (query.next()) {
                        Document doc;



                        QString id = query.value(0).toString();
                        QString title = query.value(1).toString();

                        TCHAR *Tcharid =  QStringToTChar(id);
                        TCHAR *TcharTitle = QStringToTChar(title);
                        doc.add( *_CLNEW Field(_T("id"), Tcharid, Field::STORE_YES | Field::INDEX_UNTOKENIZED ) );
                        doc.add( *_CLNEW Field(_T("Título"),TcharTitle, Field::STORE_YES | Field::INDEX_TOKENIZED) );

                        if(indexContentField == true)
                        {
                            QByteArray datos =  query.value(2).toByteArray();

                            bool CompressionError=false;
                            datos = gUncompress(datos,CompressionError);

                            if(CompressionError==false){

//#ifdef IndexContentDebug
                                qDebug()<<"error al descomprimir FD";
//#endif
                            }
                            else {
                                datos =  query.value(2).toByteArray();
                                 }
 ;
                                qDebug()<<datos;
                                QString content = CleanHtml(QString(datos));

                                TCHAR *TcharContent = QStringToTChar(content);
                                doc.add( *_CLNEW Field(_T("Contenido"), TcharContent, Field::STORE_NO | Field::INDEX_TOKENIZED) );

                                writer->addDocument(&doc);
                                doc.clear();
                                delete [] Tcharid;
                                delete [] TcharTitle;
                                delete [] TcharContent;

                            }
                            else {

                            writer->addDocument(&doc);
                            doc.clear();
                            delete [] Tcharid;
                            delete [] TcharTitle;

                                }

                        contador++;
                    }

                    start += size;

                    if(start>contador){
                        static int c = 0;
                        if(c==1){
                            contador = total;
                            emit Error(ErrLucene,"Error desconocido","");
                        }
                        c++;
                    }
                    emit indexContentProgress(contador);

                }

                emit indexContentFinished();

                writer->setUseCompoundFile(true);
                writer->optimize();
            } else {
                emit Error(ErrBd,"Error al abrir la base de datos",db.lastError().text());
                emit indexContentFinished();
            }

        }


    } catch (CLuceneError err) {

        emit Error(ErrLucene,toError(err.number()),QString(err.what()));
        emit indexContentFinished();
    }
    QSqlDatabase::removeDatabase("info_s");

    if(writer!=NULL){
        writer->close();
        _CLLDELETE(writer);
    }
    //mutex.unlock();
}



void CLuceneHelper::Search(const QString &cad,size_t limite){

    QList<QPair<QString, int> > records;

    QString cad1 = cad;

    if(isAvailable()){;

        QPair<QString, int> item;

        //QMutex mutex;
        //mutex.lock();

        try {

            IndexReader * reader = IndexReader::open(DirIndex.toLatin1().constData(),false);
            IndexSearcher s(reader);

            Query* q;
            TCHAR *QueryString =  QStringToTChar(cad);

            if(!MultiFieldSearch || (this->getFieldNames().count()<2)){
                QRegExp rx1("(Título|Contenido)(:)");
                TCHAR *TCharSearchField =  QStringToTChar(SearchField);

                q = QueryParser::parse(QueryString,TCharSearchField,&an);

                if(!cad.contains(rx1)){
                    q = QueryParser::parse(QueryParser::escape(QueryString),TCharSearchField,&an);
                } else {                    
                    q = QueryParser::parse(QueryString,TCharSearchField,&an);
                }


                //if()


                SearchField = "Título";
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

            if(limite != 0)
                if(cant_doc > limite) cant_doc = limite;

            for ( size_t i=0;i<cant_doc;i++ ){
                Document* doc = &h->doc(i);
                QString title = TCharToQString(doc->get(_T("Título")));
                QString id = TCharToQString(doc->get(_T("id")));

                item.first = title;
                item.second = id.toInt();
                records.append(item);
            }
            t2 = clock();
            double tiempo = double( t2 - t1 ) / (double)CLOCKS_PER_SEC;

            _CLLDELETE(h);
            _CLLDELETE(q);

            s.close();
            reader->close();
            _CLLDELETE(reader);

            emit OnSearchFinish(records,tiempo);

        } catch (CLuceneError err) {
            QString StrExcept =  QString(err.what()).toUtf8();
            emit Error(ErrLucene,toError(err.number()),StrExcept);
            emit OnSearchFinish(records,0);
        }

        //mutex.unlock();
    } else {
        emit Error(ErrLucene,"El índice de búsqueda no es válido","");
        emit OnSearchFinish(records,0);
    }
}

bool CLuceneHelper::isAvailable(){

    bool valid = true;

    if(!QFile::exists(DirIndex)){
        QString ErrStr = QString("El índice de búsqueda %1 no existe o está dañado").arg(DirIndex);        
        emit Error(ErrFile,ErrStr,"");
        return false;
    }


    if(!IndexReader::indexExists(DirIndex.toLatin1().constData())){
        QString ErrStr = QString("El indice %1 no existe").arg(DirIndex);
        emit Error(ErrFile,ErrStr,"");
        return false;
    }

    return valid;
}

QString CLuceneHelper::CleanHtml(const QString &cad) const{

    QString textString;

    textString = cad;

    return textString.replace(QRegExp("<[^>]*>"),"");
}

QString CLuceneHelper::toError(int number) const {

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

QStringList CLuceneHelper::getFieldNames() {

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
        emit Error(ErrLucene,toError(err.number()),QString(err.what()));
    }
    return QStringList();
}

QString CLuceneHelper::LastQuery() const{
    return LastQueryString;
}

void CLuceneHelper::SetDirIndex(const QString &Dir)
{
    DirIndex = Dir + "s_index";
}

void CLuceneHelper::SetDataBaseName(const QString &DbName)
{
    DataBase = DbName;
}

QString CLuceneHelper::GetDirIndex() const
{
    return DirIndex;
}

QString CLuceneHelper::GetDataBaseName() const
{
    return DataBase;
}

void CLuceneHelper::UseMultiFieldSearch(void){
    MultiFieldSearch = true;
}

void CLuceneHelper::SetSearchField (const QString Field){
    SearchField = Field;
}
