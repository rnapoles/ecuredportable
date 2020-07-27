#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QTextCodec>

#include <QRegExp>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <CLucene.h>
#include <CLucene/StdHeader.h>
#include <CLucene/util/Misc.h>

#include <zlib.h>

#include <iostream>

using namespace std;
using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::queryParser;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::search;


TCHAR* QStringToTChar(const QString &str)
{
    TCHAR *string = new TCHAR[(str.length() +1) * sizeof(TCHAR)];
    memset(string, 0, (str.length() +1) * sizeof(TCHAR));
#if defined(UNICODE) || defined(_CL_HAVE_WCHAR_H) && defined(_CL_HAVE_WCHAR_T)
    str.toWCharArray(string);
#else
    const QByteArray ba = str.toAscii();
    strcpy(string, ba.constData());
#endif
    return string;
}

QString TCharToQString(const TCHAR *string)
{
#if defined(UNICODE) || defined(_CL_HAVE_WCHAR_H) && defined(_CL_HAVE_WCHAR_T)
    QString retValue = QString::fromWCharArray(string);
    return retValue;
#else
    return QString(QLatin1String(string));
#endif
}

QByteArray gUncompress(const QByteArray &data,bool &error)
{
     error = false;
    if (data.size() <= 4) {
        qWarning("gUncompress: Input data is truncated");
        error = true;
        return QByteArray();
    }

    QByteArray result;

    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in = (Bytef*)(data.data());

    ret = inflateInit2(&strm, 15 +  32); // gzip decoding
    if (ret != Z_OK){
        error = true;
        return QByteArray();
    }

    // run inflate()
    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered

        switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
             error = true;
            return QByteArray();
        }

        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    return result;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "init test";

    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Aplicacion.sqlite");

    //Abre la base de datos y muestra un aviso en caso de error
    if (!db.open()) {
        qDebug()<<"No se pudo abrir la bd";
        return false;
    }

    int start = 0;
    int size = 100;
    int contador = 0;
    //int total = 68337;
    int total = 3000;

    try {
        lucene::analysis::standard::StandardAnalyzer an;
        //IndexWriter *writer = new IndexWriter("archivos",&an,false,true);
        IndexWriter *writer = _CLNEW IndexWriter("DirIndex",&an,true);


        writer->setMaxFieldLength(0x7FFFFFFFL);
        writer->setUseCompoundFile(false);

        //indexar doc

        Document doc;

         while (contador < total ){
            QString strQuery = QString("SELECT docid,content,old_text FROM  wikitext JOIN searchindex ON old_id=docid LIMIT %1,%2;")
                                    .arg(start).arg(size);


            //qDebug()<< strQuery;

            QSqlQuery query(strQuery,db);

            while (query.next()) {
                QString id = query.value(0).toString();
                QString title = query.value(1).toString();

                QByteArray datos =  query.value(2).toByteArray();
                //QString str1;
                bool CompressionError=false;
                datos = gUncompress(datos,CompressionError);
                if(CompressionError==true){
                    //str1=QString(datos);
                    //qDebug()<<query.value(2).toByteArray();
                    qDebug()<<"Error de compresion";
                }

                //QString content(datos)    ;
                QString content = QString(datos).replace(QRegExp("<[^>]*>"),"");


                qDebug() << id;
                doc.clear();
                doc.add( *_CLNEW Field(_T("id"), QStringToTChar(id), Field::STORE_YES | Field::INDEX_UNTOKENIZED ) );
                doc.add( *_CLNEW Field(_T("title"), QStringToTChar(title), Field::STORE_YES | Field::INDEX_TOKENIZED) );

                Field* f = _CLNEW Field(_T("content"), QStringToTChar(content), Field::STORE_NO | Field::INDEX_TOKENIZED);
                f->setOmitNorms(true);
                doc.add(*f);
                //_CLLDELETE(f);
                //doc.add( *_CLNEW Field(_T("content"), QStringToTChar(content), Field::STORE_NO | Field::INDEX_TOKENIZED) );
                //delete datos;
                //delete content;


                writer->addDocument(&doc);
                contador++;
            }

            if(query.lastError().type()!=0) {
                qDebug() << query.lastError();
                break;
            }



            start += size;
        }

        writer->setUseCompoundFile(true);
        writer->optimize();
        writer->close();

        _CLLDELETE(writer);
        _lucene_shutdown();


    } catch (CLuceneError err) {
        qDebug() << "Error:"<< err.what();

        int i;
        cin >> i;

    }

    qDebug() << "Finish test";
    return a.exec();
}
