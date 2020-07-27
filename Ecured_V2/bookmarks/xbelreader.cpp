
#include "xbelreader.h"
#include <qfile.h>
#include "bookmarknode.h"

QString XmlEntityResolver::resolveUndeclaredEntity(const QString &entity)
{
    if (entity == QLatin1String("nbsp"))
        return QLatin1String(" ");

    return QString();
}

XbelReader::XbelReader()
    : m_entityResolver(0)
{
    m_entityResolver = new XmlEntityResolver();
    setEntityResolver(m_entityResolver);
}

XbelReader::~XbelReader()
{
    delete m_entityResolver;
}

BookmarkNode *XbelReader::read(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        return new BookmarkNode(BookmarkNode::Root);
    }
    file.open(QFile::ReadOnly);
    return read(&file);
}

BookmarkNode *XbelReader::read(QIODevice *device)
{
    BookmarkNode *root = new BookmarkNode(BookmarkNode::Root);
    setDevice(device);
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            QString version = attributes().value(QLatin1String("version")).toString();
            if (name() == QLatin1String("xbel")
                && (version.isEmpty() || version == QLatin1String("1.0"))) {
                readXBEL(root);
            } else {
                raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
            }
        }
    }
    return root;
}

void XbelReader::readXBEL(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("xbel"));

    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("folder"))
                readFolder(parent);
            else if (name() == QLatin1String("bookmark"))
                readBookmarkNode(parent);
            else if (name() == QLatin1String("separator"))
                readSeparator(parent);
            else
                skipUnknownElement();
        }
    }
}

void XbelReader::readFolder(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("folder"));

    BookmarkNode *folder = new BookmarkNode(BookmarkNode::Folder, parent);
    folder->expanded = (attributes().value(QLatin1String("folded")) == QLatin1String("no"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("title"))
                readTitle(folder);
            else if (name() == QLatin1String("desc"))
                readDescription(folder);
            else if (name() == QLatin1String("folder"))
                readFolder(folder);
            else if (name() == QLatin1String("bookmark"))
                readBookmarkNode(folder);
            else if (name() == QLatin1String("separator"))
                readSeparator(folder);
            else
                skipUnknownElement();
        }
    }
}

void XbelReader::readTitle(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("title"));
    parent->title = readElementText();
}

void XbelReader::readDescription(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("desc"));
    parent->desc = readElementText();
}

void XbelReader::readSeparator(BookmarkNode *parent)
{
    new BookmarkNode(BookmarkNode::Separator, parent);
    // empty elements have a start and end element
    readNext();
}

void XbelReader::readBookmarkNode(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("bookmark"));
    BookmarkNode *bookmark = new BookmarkNode(BookmarkNode::Bookmark, parent);
    bookmark->url = attributes().value(QLatin1String("href")).toString();
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("title"))
                readTitle(bookmark);
            else if (name() == QLatin1String("desc"))
                readDescription(bookmark);
            else
                skipUnknownElement();
        }
    }
    if (bookmark->title.isEmpty())
        bookmark->title = QObject::tr("Unknown title");
}

void XbelReader::skipUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            skipUnknownElement();
    }
}

