#ifndef WIKIDBIMPORTER_H
#define WIKIDBIMPORTER_H

#include <QObject>

class WikiDbImporter : public QObject
{
    Q_OBJECT
public:
    explicit WikiDbImporter(QObject *parent = 0);
    void SetSource(const QString &s);
    void SetDestination(const QString &s);

signals:
    void ReportProgress(int progress);
    void Init();
    void Finish();
    void Error(const QString &Msg);
    void ReportMsg(const QString &Msg);
    void FileSize(int size);


public slots:
    void copy();
private:
    QString source;
    QString destination;
    void copyFile(const QString &source, QString &destination, bool overwrite, bool move);
    
};

#endif // WIKIDBIMPORTER_H
