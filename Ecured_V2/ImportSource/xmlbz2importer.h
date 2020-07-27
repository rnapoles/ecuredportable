#ifndef XMLBZ2IMPORTER_H
#define XMLBZ2IMPORTER_H

#include <QObject>

class XmlBz2Importer : public QObject
{
    Q_OBJECT
public:
    explicit XmlBz2Importer(QObject *parent = 0);
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

private slots:
    void SlotReportProgress(int progress);
    void SlotError(const QString &Msg);
    void SlotReportMsg(const QString &Msg);


private:
    QString source;
    QString destination;
    
};

#endif // XMLBZ2IMPORTER_H
