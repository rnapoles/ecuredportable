#ifndef CLUCENEHELPER_H
#define CLUCENEHELPER_H

#include <QObject>
#include <QList>
#include <QPair>

#include <CLucene.h>
#include <CLucene/StdHeader.h>
#include <CLucene/util/Misc.h>
#include <CLucene/snowball/SnowballAnalyzer.h>
#include <QSqlError>
#include <CommonDefines.h>

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::queryParser;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::search;

//#define ErrBd 1
//#define ErrLucene 2


class CLuceneHelper : public QObject
{
    Q_OBJECT
public:
    static CLuceneHelper *getInstance()
    {
        if (!s_instance)
          s_instance = new CLuceneHelper;
        return s_instance;
    }

    ~CLuceneHelper();
    void indexContent(bool indexContentField=false);
    void Search(const QString &cad,size_t limite=0);
    bool isAvailable();
    QString CleanHtml(const QString &cad) const;
    QString toError(const int) const;
    QStringList getFieldNames() ;
    QString LastQuery() const;

    void UseMultiFieldSearch(void){
        MultiFieldSearch = true;
    }

    void SetSearchField (const QString Field){
        SearchField = Field;
    }

    void SetDirIndex(const QString &Dir);
    void SetDataBaseName(const QString &DbName);
    QString GetDirIndex() const;
    QString GetDataBaseName()const;

private:

    static CLuceneHelper *s_instance;
    QString DirIndex;
    QString DataBase;
    snowball::SnowballAnalyzer an;
    bool MultiFieldSearch;
    QString SearchField;
    QString LastQueryString;
    explicit CLuceneHelper(QObject *parent = 0);


signals:
    void OnSearchFinish(const QList<QPair<QString, int> >& results,double tiempo);
    void indexContentStarted(int max);
    void indexContentProgress(int progress);
    void indexContentFinished();
    void Error(const int ErrType,const QString Msg,const QString OrgMsg);

public slots:

};

#endif // CLUCENEHELPER_H
