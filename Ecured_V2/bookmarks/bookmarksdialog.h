
#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <qdialog.h>
#include "ui_bookmarksdialog.h"

//#include "tabwidget.h"

#include <qabstractitemmodel.h>
#include <qurl.h>

#include "bookmarksmanager.h"

class TreeSortFilterProxyModel;
class BookmarksManager;
class BookmarksModel;
class BookmarkNode;
class BookmarksDialog : public QDialog, public Ui_BookmarksDialog
{
    Q_OBJECT

signals:
    void openUrl(const QUrl &url, OpenUrlIn tab, const QString &title);

public:
    BookmarksDialog(QWidget *parent = 0, BookmarksManager *manager = 0);
    ~BookmarksDialog();

private slots:
    void customContextMenuRequested(const QPoint &pos);
    void openBookmark(OpenUrlIn tab);
    void openBookmark();
    void openInNewTab();
    void openInCurrentTab();
    void editName();
    void editAddress();
    void newFolder();

private:
    void expandNodes(BookmarkNode *node);
    bool saveExpandedNodes(const QModelIndex &parent);

    BookmarksManager *m_bookmarksManager;
    BookmarksModel *m_bookmarksModel;
    TreeSortFilterProxyModel *m_proxyModel;
};

#endif // BOOKMARKSDIALOG_H
