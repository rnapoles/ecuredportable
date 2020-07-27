#include "reporter.h"
#include <QProcess>
#include <QFile>
#include <QDir>

void Reporter::Error(const QString & id,const QString & dump_path)
{
    qDebug()<<id;
    qDebug()<<dump_path;

    QString path = this->path;
#if defined(Q_OS_WIN32)
    path.append("ErrorReporter.exe");
#else
    path.append("ErrorReporter");
#endif
    path =  QDir::toNativeSeparators(path);
    if(QFile::exists(path)){
        QProcess::startDetached(path);
    }
}

void Reporter::setPath(const QString &thepath)
{
    this->path=thepath;
}
