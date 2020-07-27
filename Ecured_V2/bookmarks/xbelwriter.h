
#ifndef XBELWRITER_H
#define XBELWRITER_H

#include <qxmlstream.h>
#include <qdatetime.h>

class BookmarkNode;
class XbelWriter : public QXmlStreamWriter
{
public:
    XbelWriter();
    bool write(const QString &fileName, const BookmarkNode *root);
    bool write(QIODevice *device, const BookmarkNode *root);

private:
    void writeItem(const BookmarkNode *parent);
};

#endif // XBELWRITER_H

