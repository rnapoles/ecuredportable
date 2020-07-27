
#ifndef XBELREADER_H
#define XBELREADER_H

#include <qxmlstream.h>
#include <qdatetime.h>

class BookmarkNode;
class XmlEntityResolver : public QXmlStreamEntityResolver
{
public:
    QString resolveUndeclaredEntity(const QString &entity);
};

class XbelReader : public QXmlStreamReader
{
public:
    XbelReader();
    ~XbelReader();

    BookmarkNode *read(const QString &fileName);
    BookmarkNode *read(QIODevice *device);

private:
    void skipUnknownElement();
    void readXBEL(BookmarkNode *parent);
    void readTitle(BookmarkNode *parent);
    void readDescription(BookmarkNode *parent);
    void readSeparator(BookmarkNode *parent);
    void readFolder(BookmarkNode *parent);
    void readBookmarkNode(BookmarkNode *parent);

private:
    XmlEntityResolver *m_entityResolver;
};

#endif // XBELREADER_H

