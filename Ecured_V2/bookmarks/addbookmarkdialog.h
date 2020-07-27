

#ifndef ADDBOOKMARKDIALOG_H
#define ADDBOOKMARKDIALOG_H

#include "ui_addbookmarkdialog.h"

#include <qsortfilterproxymodel.h>

class AddBookmarkProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AddBookmarkProxyModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class BookmarkNode;
class BookmarksManager;
class QTreeView;
class AddBookmarkDialog : public QDialog, public Ui_AddBookmarkDialog
{
    Q_OBJECT

public:
    AddBookmarkDialog(QWidget *parent = 0, BookmarksManager *bookmarksManager = 0);

    void setUrl(const QString &url);
    QString url() const;

    void setTitle(const QString &title);
    QString title() const;

    void setCurrentIndex(const QModelIndex &index);
    QModelIndex currentIndex() const;

    void setFolder(bool addFolder);
    bool isFolder() const;

    BookmarkNode *addedNode() const;

    void setm_bookmarksManager(BookmarksManager *m_bookmarksManager);
    BookmarksManager *get_m_bookmarksManager();

private slots:
    void accept();

private:
    QTreeView *m_treeView;
    BookmarksManager *m_bookmarksManager;
    BookmarkNode *m_addedNode;
    AddBookmarkProxyModel *m_proxyModel;
    bool m_addFolder;
};

#endif // ADDBOOKMARKDIALOG_H
