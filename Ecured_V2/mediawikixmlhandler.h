#ifndef MEDIAWIKIXMLHANDLER_H
#define MEDIAWIKIXMLHANDLER_H

#include <QObject>
#include <QXmlDefaultHandler>
#include <databasemanager.h>

class MediawikiXmlHandler :public QObject, public QXmlDefaultHandler
{

public:
    MediawikiXmlHandler(const QString &db);
    ~MediawikiXmlHandler();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const;

private:
    QString currentText;
    QString errorStr;
    QString Title;
    QString Text;
    bool initErr;
    bool IsTitle;
    bool IsText;
    DatabaseManager* dbm;
    Q_OBJECT

signals:

    void Error(const QString &Msg);
    void ReportMsg(const QString &Msg);
};

#endif // MEDIAWIKIXMLHANDLER_H
