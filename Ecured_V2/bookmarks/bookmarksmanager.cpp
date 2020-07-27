#include "bookmarksmanager.h"
#include "autosaver.h"
#include "bookmarknode.h"
#include "bookmarksmodel.h"
#include "xbelreader.h"
#include "xbelwriter.h"
#include <qbuffer.h>
#include <qdesktopservices.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qheaderview.h>
#include <qicon.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qtoolbutton.h>
#include <qprocess.h>
#include <qwebsettings.h>
#include <QCoreApplication>
#include <QDir>
#include <qdebug.h>

BookmarksManager::BookmarksManager(QObject *parent)
    : QObject(parent)
    , m_loaded(false)
    , m_saveTimer(new AutoSaver(this))
    , m_bookmarkRootNode(0)
    //, m_toolbar(0)
    , m_menu(0)
{

    connect(this, SIGNAL(entryAdded(BookmarkNode *)),
            m_saveTimer, SLOT(changeOccurred()));
    connect(this, SIGNAL(entryRemoved(BookmarkNode *, int, BookmarkNode *)),
            m_saveTimer, SLOT(changeOccurred()));
    connect(this, SIGNAL(entryChanged(BookmarkNode *)),
            m_saveTimer, SLOT(changeOccurred()));
    m_bookmarkModel = new BookmarksModel(this, this);
}

BookmarksManager::~BookmarksManager()
{
    m_saveTimer->saveIfNeccessary();
    delete m_bookmarkRootNode;
}

void BookmarksManager::changeExpanded()
{
    m_saveTimer->changeOccurred();
}

void  BookmarksManager::forceload(){
 m_loaded = false;
 load();
}
void BookmarksManager::load()
{
    if (m_loaded)
        return;
    m_loaded = true;

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString bookmarkFile = dir + QLatin1String("/bookmarks.xbel");


    if (!QFile::exists(bookmarkFile)){
        QDir d;
        d.mkpath(dir);

        QXmlStreamWriter stream(&bookmarkFile);
        stream.setAutoFormatting(true);
    }
    XbelReader reader;
    m_bookmarkRootNode = reader.read(bookmarkFile);
    if (reader.error() != QXmlStreamReader::NoError) {
        QMessageBox::warning(0, QLatin1String("Loading Bookmark"),
            tr("Error when loading bookmarks on line %1, column %2:\n"
               "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
    }

    QList<BookmarkNode*> others;
    for (int i = m_bookmarkRootNode->children().count() - 1; i >= 0; --i) {
        BookmarkNode *node = m_bookmarkRootNode->children().at(i);
        if (node->type() == BookmarkNode::Folder) {

            // Automatically convert
            if ((node->title == tr("Menu")
                 || node->title == QLatin1String("Marcadores")) && !m_menu) {
                node->title = tr("Marcadores");
                m_menu = node;
            }
        } else {
            others.append(node);
        }
        m_bookmarkRootNode->remove(node);
    }
    Q_ASSERT(m_bookmarkRootNode->children().count() == 0);

    if (!m_menu) {
        m_menu = new BookmarkNode(BookmarkNode::Folder, m_bookmarkRootNode);
        m_menu->title =  "Marcadores" ;
    } else {
        m_bookmarkRootNode->add(m_menu);
    }

    for (int i = 0; i < others.count(); ++i)
        m_menu->add(others.at(i));
}

void BookmarksManager::save()
{
    if (!m_loaded)
        return;

    XbelWriter writer;
    QString dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString bookmarkFile = dir + QLatin1String("/bookmarks.xbel");
    // Save root folder titles in English (i.e. not localized)
    m_menu->title = QLatin1String("Marcadores");
    if (!writer.write(bookmarkFile, m_bookmarkRootNode))
        qWarning() << "BookmarkManager: error saving to" << bookmarkFile;


}

void BookmarksManager::retranslate() const
{
    if (m_menu)
        m_menu->title = "Marcadores";
}

void BookmarksManager::addBookmark(BookmarkNode *parent, BookmarkNode *node, int row)
{
    if (!m_loaded){

        return;
    }


    Q_ASSERT(parent);
    InsertBookmarksCommand *command = new InsertBookmarksCommand(this, parent, node, row);
    m_commands.push(command);
}

void BookmarksManager::removeBookmark(BookmarkNode *node)
{
    if (!m_loaded)
        return;

    Q_ASSERT(node);
    BookmarkNode *parent = node->parent();
    int row = parent->children().indexOf(node);
    RemoveBookmarksCommand *command = new RemoveBookmarksCommand(this, parent, row);
    m_commands.push(command);
}

void BookmarksManager::setTitle(BookmarkNode *node, const QString &newTitle)
{
    if (!m_loaded)
        return;

    Q_ASSERT(node);
    ChangeBookmarkCommand *command = new ChangeBookmarkCommand(this, node, newTitle, true);
    m_commands.push(command);
}

void BookmarksManager::setUrl(BookmarkNode *node, const QString &newUrl)
{
    if (!m_loaded)
        return;

    Q_ASSERT(node);
    ChangeBookmarkCommand *command = new ChangeBookmarkCommand(this, node, newUrl, false);
    m_commands.push(command);
}

BookmarkNode *BookmarksManager::bookmarks()
{
    if (!m_loaded)
        load();
    return m_bookmarkRootNode;
}

BookmarkNode *BookmarksManager::menu()
{
    if (!m_loaded)
        load();

    Q_ASSERT(m_menu);
    return m_menu;
}

BookmarksModel *BookmarksManager::bookmarksModel()
{
    //int i=0;
    if (m_bookmarkModel==NULL){

        m_bookmarkModel = new BookmarksModel(this, this);
    }
    return m_bookmarkModel;
}

void BookmarksManager::importBookmarks()
{
    QStringList supportedFormats;
    supportedFormats << tr("XBEL bookmarks").append(QLatin1String("(*.xbel *.xml)"));
    supportedFormats << tr("HTML Netscape bookmarks").append(QLatin1String("(*.html)"));

    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    QString(), supportedFormats.join(QLatin1String(";;")));
    if (fileName.isEmpty())
        return;

    XbelReader reader;
    BookmarkNode *importRootNode = 0;
    if (fileName.endsWith(QLatin1String(".html"))) {
        QString program = QLatin1String("htmlToXBel");
        QStringList arguments;
        arguments << fileName;
        QProcess process;
        process.start(program, arguments);
        process.waitForFinished(-1);
        if (process.error() != QProcess::UnknownError) {
            if (process.error() == QProcess::FailedToStart) {
                QMessageBox::warning(0, tr("htmlToXBel tool required"),
                    tr("htmlToXBel tool, which is shipped with Arora and is needed to import HTML bookmarks, "
                       "is not installed or not available in the search paths."));
            } else {
                QMessageBox::warning(0, tr("Loading Bookmark"),
                    tr("Error when loading HTML bookmarks: %1\n").arg(process.errorString()));
            }
            return;
        }
        importRootNode = reader.read(&process);
    } else {
        importRootNode = reader.read(fileName);
    }
    if (reader.error() != QXmlStreamReader::NoError) {
        QMessageBox::warning(0, QLatin1String("Loading Bookmark"),
            tr("Error when loading bookmarks on line %1, column %2:\n"
               "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
        delete importRootNode;
        return;
    }

    importRootNode->setType(BookmarkNode::Folder);
    importRootNode->title = (tr("Imported %1").arg(QDate::currentDate().toString(Qt::SystemLocaleShortDate)));
    addBookmark(menu(), importRootNode);
}

void BookmarksManager::exportBookmarks()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save File"),
                                tr("%1 Bookmarks.xbel").arg(QCoreApplication::applicationName()),
                                tr("XBEL bookmarks").append(QLatin1String("(*.xbel *.xml)")));
    if (fileName.isEmpty())
        return;

    XbelWriter writer;
    if (!writer.write(fileName, m_bookmarkRootNode))
        QMessageBox::critical(0, tr("Export error"), tr("error saving bookmarks"));
}

RemoveBookmarksCommand::RemoveBookmarksCommand(BookmarksManager *m_bookmarkManagaer, BookmarkNode *parent, int row)
    : QUndoCommand(BookmarksManager::tr("Remove Bookmark"))
    , m_row(row)
    , m_bookmarkManagaer(m_bookmarkManagaer)
    , m_node(parent->children().value(row))
    , m_parent(parent)
    , m_done(false)
{
}

RemoveBookmarksCommand::~RemoveBookmarksCommand()
{
    if (m_done && !m_node->parent()) {
        delete m_node;
    }
}

void RemoveBookmarksCommand::undo()
{
    m_parent->add(m_node, m_row);
    emit m_bookmarkManagaer->entryAdded(m_node);
    m_done = false;
}

void RemoveBookmarksCommand::redo()
{
    m_parent->remove(m_node);
    emit m_bookmarkManagaer->entryRemoved(m_parent, m_row, m_node);
    m_done = true;
}

InsertBookmarksCommand::InsertBookmarksCommand(BookmarksManager *m_bookmarkManagaer,
                BookmarkNode *parent, BookmarkNode *node, int row)
    : RemoveBookmarksCommand(m_bookmarkManagaer, parent, row)
{
    setText(BookmarksManager::tr("Insert Bookmark"));
    m_node = node;
}

ChangeBookmarkCommand::ChangeBookmarkCommand(BookmarksManager *m_bookmarkManagaer, BookmarkNode *node,
                        const QString &newValue, bool title)
    : QUndoCommand()
    , m_bookmarkManagaer(m_bookmarkManagaer)
    , m_title(title)
    , m_newValue(newValue)
    , m_node(node)
{
    if (m_title) {
        m_oldValue = m_node->title;
        setText(BookmarksManager::tr("Name Change", "Undo bookmark title change"));
    } else {
        m_oldValue = m_node->url;
        setText(BookmarksManager::tr("Address Change", "Undo bookmark url change"));
    }
}

void ChangeBookmarkCommand::undo()
{
    if (m_title)
        m_node->title = m_oldValue;
    else
        m_node->url = m_oldValue;
    emit m_bookmarkManagaer->entryChanged(m_node);
}

void ChangeBookmarkCommand::redo()
{
    if (m_title)
        m_node->title = m_newValue;
    else
        m_node->url = m_newValue;
    emit m_bookmarkManagaer->entryChanged(m_node);
}

