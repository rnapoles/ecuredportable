
#include "addbookmarkdialog.h"

#include "bookmarknode.h"
#include "bookmarksmanager.h"
#include "bookmarksmodel.h"
#include <QAbstractButton>

#include <qheaderview.h>
#include <qtreeview.h>
#include <QMessageBox>

AddBookmarkProxyModel::AddBookmarkProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

int AddBookmarkProxyModel::columnCount(const QModelIndex &parent) const
{
    return qMin(1, QSortFilterProxyModel::columnCount(parent));
}

bool AddBookmarkProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    return sourceModel()->hasChildren(idx);
}

AddBookmarkDialog::AddBookmarkDialog(QWidget *parent, BookmarksManager *bookmarksManager)
    : QDialog(parent)
    , m_bookmarksManager(bookmarksManager)
    , m_addedNode(0)
    , m_proxyModel(0)
    , m_addFolder(false)
{
    // se modifico el constructor de la accion annadire marcador.
   setWindowFlags(Qt::Sheet);

   if (!m_bookmarksManager)
       m_bookmarksManager = get_m_bookmarksManager();

   setupUi(this);

   m_treeView = new QTreeView(this);
   m_proxyModel = new AddBookmarkProxyModel(this);
   BookmarksModel *model = m_bookmarksManager->bookmarksModel();
   m_proxyModel->setSourceModel(model);

// Configuración del Idioma--(Español)
   QList<QAbstractButton *>botones = buttonBox->buttons();
   botones.at(0)->setText("&Aceptar");
   botones.at(1)->setText("&Cerrar");

   m_treeView->setModel(m_proxyModel);
   m_treeView->expandAll();
   m_treeView->header()->setStretchLastSection(true);
   m_treeView->header()->hide();
   m_treeView->setItemsExpandable(false);
   m_treeView->setRootIsDecorated(false);
   m_treeView->setIndentation(10);
   //m_treeView->setCurrentIndex(m_proxyModel->index(0,0));
   location->setModel(m_proxyModel);
   m_treeView->show();
   location->setView(m_treeView);

   BookmarkNode*menu =m_bookmarksManager->menu();
   QModelIndex idx = m_proxyModel->mapFromSource(model->index(menu));
   m_treeView->setCurrentIndex(idx);
   location->setCurrentIndex(idx.row());




   address->setInactiveText(tr("Url"));
   name->setInactiveText("Título");

   resize(sizeHint());
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void AddBookmarkDialog::setUrl(const QString &url)
{
    address->setText(url);
    address->setVisible(url.isEmpty());
    resize(sizeHint());
}

void AddBookmarkDialog::setm_bookmarksManager(BookmarksManager *m_bookmarksManager){

    m_bookmarksManager = m_bookmarksManager;
}
BookmarksManager *AddBookmarkDialog::get_m_bookmarksManager(){
    return m_bookmarksManager;
}

QString AddBookmarkDialog::url() const
{
    return address->text();
}

void AddBookmarkDialog::setTitle(const QString &title)
{
    name->setText(title);
}

QString AddBookmarkDialog::title() const
{
    return name->text();
}

void AddBookmarkDialog::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid()){

        return;
    }

    QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
    m_treeView->setCurrentIndex(proxyIndex);
    location->setCurrentIndex(proxyIndex.row());
}

QModelIndex AddBookmarkDialog::currentIndex() const
{
    QModelIndex index = location->view()->currentIndex();
    index = m_proxyModel->mapToSource(index);
    return index;
}

void AddBookmarkDialog::setFolder(bool addFolder)
{
    m_addFolder = addFolder;

    if (addFolder) {
        setWindowTitle(tr("Añadir carpeta"));
        address->setVisible(false);
    } else {
        setWindowTitle(tr("Añadir marcador"));
        address->setVisible(true);
    }

    resize(sizeHint());
}

bool AddBookmarkDialog::isFolder() const
{
    return m_addFolder;
}

BookmarkNode *AddBookmarkDialog::addedNode() const
{
    return m_addedNode;
}

void AddBookmarkDialog::accept()
{
    if ((!m_addFolder && address->text().isEmpty()) || name->text().isEmpty()) {
        QDialog::accept();
        return;
    }

    QModelIndex index = currentIndex();
    if (!index.isValid()){
        index = m_bookmarksManager->bookmarksModel()->index(0, 0);
    }

    BookmarkNode *parent = m_bookmarksManager->bookmarksModel()->node(index);

    BookmarkNode::Type type = (m_addFolder) ? BookmarkNode::Folder : BookmarkNode::Bookmark;
    BookmarkNode *bookmark = new BookmarkNode(type);
    bookmark->title = name->text();
    if (!m_addFolder)
        bookmark->url = address->text();

    m_bookmarksManager->addBookmark(parent, bookmark);
    m_addedNode = bookmark;
    QDialog::accept();
}

