#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include <QDebug>

class Reporter : public QObject
 {
     Q_OBJECT

private:
    QString path;

public:
    void setPath(const QString &thepath);

 public slots:
    void Error(const QString & id,const QString & path);

};

#endif // REPORTER_H
