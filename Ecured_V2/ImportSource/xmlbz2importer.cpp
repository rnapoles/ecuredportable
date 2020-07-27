#include "xmlbz2importer.h"
#include <QFile>
#include "qbzip2device.h"
#include <mediawikixmlhandler.h>

XmlBz2Importer::XmlBz2Importer(QObject *parent) :
    QObject(parent)
{
}

void XmlBz2Importer::SetSource(const QString &s)
{
    this->source=s;
}

void XmlBz2Importer::SetDestination(const QString &s)
{
    this->destination=s;
}

void XmlBz2Importer::copy()
{
    emit Init();

    QFile file(this->source);
    emit FileSize(file.size());

    QBZip2Device compressor(&file);
    compressor.open(QIODevice::ReadOnly);

    MediawikiXmlHandler handler(this->destination);

    connect(&handler,SIGNAL(Error(QString)),this,SLOT(SlotError(QString)));
    connect(&handler,SIGNAL(ReportMsg(QString)),this,SLOT(SlotReportMsg(QString)));
    connect(&compressor,SIGNAL(CurrentPos(int)),this,SLOT(SlotReportProgress(int)));

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QXmlInputSource *source = new QXmlInputSource(&compressor);

    bool ok = reader.parse(source);
    if(!ok) emit Error("Error al procesar el archivo");
    emit Finish();
}

void XmlBz2Importer::SlotReportProgress(int progress)
{
    emit ReportProgress(progress);
}

void XmlBz2Importer::SlotError(const QString &Msg)
{
    emit Error(Msg);
}

void XmlBz2Importer::SlotReportMsg(const QString &Msg)
{
    emit ReportMsg(Msg);
}

