
#include "bookmarksmenu.h"
#include "bookmarknode.h"
#include "bookmarksmanager.h"
#include "bookmarksmodel.h"
#include "mainwindow.h"


BookmarksMenu::BookmarksMenu(QWidget *parent)
    : ModelMenu(parent)
{
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(activated(const QModelIndex &)));
    setStatusBarTextRole(BookmarksModel::UrlStringRole);
    setSeparatorRole(BookmarksModel::SeparatorRole);
}


ModelMenu *BookmarksMenu::createBaseMenu()
{
    BookmarksMenu *menu = new BookmarksMenu(this);
    return menu;
}

void BookmarksMenu::activated(const QModelIndex &index)
{
 EcuredMainWindow *mainw = EcuredMainWindow::instance();
 QString url = index.data(BookmarksModel::UrlRole).toString();
 mainw->CreateTab(url,true);

}


void BookmarksMenu::mousePressEvent(QMouseEvent *event)
{
    QAction *c = actionAt(event ->pos());

        if (event->button() == Qt::RightButton && c ) {

            this ->Popup(event ->pos());
            event->accept();

        }
        else{

            if(c){
                QModelIndex index1 = index(c);
                QString url= index1.data(BookmarksModel::UrlRole).toString();
                EcuredMainWindow *mainWindow = EcuredMainWindow::instance();
                mainWindow->MenuMarcadoresClose();

                mainWindow->CreateCurrentTab(url);


            }

        }
 }


void BookmarksMenu::Popup(const QPoint& pt)
{
    QPoint global = this ->mapToGlobal(pt);
    QMenu* pPopup = new QMenu(this);

    QAction* pAction1 = new QAction("Abrir en una nueva pestaña", this);
    QAction* pAction2 = new QAction("Abrir", this);
    pPopup ->addAction(pAction1);
    pPopup ->addAction(pAction2);

    QAction* pItem = pPopup ->exec(global);

    if(pItem == pAction1)
    {

        QAction *c = actionAt(pt);
        QModelIndex index1 = index(c);
        QString url= index1.data(BookmarksModel::UrlRole).toString();
        EcuredMainWindow *mainWindow = EcuredMainWindow::instance();
        mainWindow->MenuMarcadoresClose();
        mainWindow->CreateTab(url,true);
    }
    else if(pItem == pAction2)
    {

        QAction *c = actionAt(pt);
        QModelIndex index1 = index(c);
        QString url= index1.data(BookmarksModel::UrlRole).toString();
        EcuredMainWindow *mainWindow = EcuredMainWindow::instance();
        mainWindow->MenuMarcadoresClose();
        mainWindow->CreateCurrentTab(url);

    }
}

void BookmarksMenu::postPopulated()
{
    if (isEmpty())
        return;

    QModelIndex parent = rootIndex();

    bool hasBookmarks = false;

    for (int i = 0; i < parent.model()->rowCount(parent); ++i) {
        QModelIndex child = parent.model()->index(i, 0, parent);

        if (child.data(BookmarksModel::TypeRole) == BookmarkNode::Bookmark) {
            hasBookmarks = true;
            break;
        }
    }

    if (!hasBookmarks)
        return;
}

void BookmarksMenu::openAll()
{
    ModelMenu *menu = qobject_cast<ModelMenu*>(sender()->parent());
    if (!menu)
        return;
    QModelIndex parent = menu->rootIndex();
    if (!parent.isValid())
        return;

    for (int i = 0; i < parent.model()->rowCount(parent); ++i) {
        QModelIndex child = parent.model()->index(i, 0, parent);

        if (child.data(BookmarksModel::TypeRole) != BookmarkNode::Bookmark)
            continue;

        emit openUrl(child.data(BookmarksModel::UrlRole).toUrl(),child.data(Qt::DisplayRole).toString());

    }
}

BookmarksMenuBarMenu::BookmarksMenuBarMenu(QWidget *parent)
    : BookmarksMenu(parent)
    , m_bookmarksManager(0)
{
}

bool BookmarksMenuBarMenu::prePopulated()
{
    setModel(m_bookmarksManager->bookmarksModel());
    setRootIndex(m_bookmarksManager->bookmarksModel()->index(m_bookmarksManager->menu()));
    // initial actions
    for (int i = 0; i < m_initialActions.count(); ++i){
        addAction(m_initialActions.at(i));
    }
    if (!m_initialActions.isEmpty())
        addSeparator();
    return true;
}

void BookmarksMenuBarMenu::setInitialActions(QList<QAction*> actions)
{
    m_initialActions = actions;
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
}

void BookmarksMenuBarMenu::setBookmarksManager(BookmarksManager *Manager){
    m_bookmarksManager = Manager;
}
