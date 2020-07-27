
#ifndef BOOKMARKSMENU_H
#define BOOKMARKSMENU_H

#include "modelmenu.h"
#include "page.h"
#include <qabstractitemmodel.h>
#include <qlist.h>
#include "bookmarksmanager.h"

//class BookmarksManager;
//class Page;

// Base class for BookmarksMenuBarMenu and BookmarksToolBarMenu
class BookmarksMenu : public ModelMenu {
    Q_OBJECT

signals:
    void openUrl(const QUrl &url, const QString &title);
    void openUrl(const QUrl &url, int tab, const QString &title);
    void openUrl(EcuredTabPage *p);
public:
    BookmarksMenu(QWidget *parent = 0);
    void Popup(const QPoint& pt);
    void mousePressEvent(QMouseEvent *event);
private:
    EcuredTabPage *bookmarkpage;

protected:
    void postPopulated();
    ModelMenu *createBaseMenu();

private slots:
    void openAll();
    void activated(const QModelIndex &index);

};

class BookmarksMenuBarMenu : public BookmarksMenu
{
    Q_OBJECT

public:
    BookmarksMenuBarMenu(QWidget *parent);
    void setInitialActions(QList<QAction*> actions);
    void setBookmarksManager(BookmarksManager *Manager);


protected:
    bool prePopulated();

private:
    BookmarksManager *m_bookmarksManager;
    QList<QAction*> m_initialActions;
};

#endif // BOOKMARKSMENU_H
