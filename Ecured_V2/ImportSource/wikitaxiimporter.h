#ifndef WIKITAXIIMPORTER_H
#define WIKITAXIIMPORTER_H

#include <QObject>

class WikiTaxiImporter : public QObject
{
    Q_OBJECT
public:
    explicit WikiTaxiImporter(QObject *parent = 0);
    void SetSource(const QString &s);
    void SetDestination(const QString &s);

signals:
    void ReportProgress(int progress);
    void Init();
    void Finish();
    void Error(const int ErrType,const QString &Msg,const QString &OrgMsg);
    void FileSize(int size);

public slots:
    void copy();


private:
    QString source;
    QString destination;
    
};

#endif // WIKITAXIIMPORTER_H
