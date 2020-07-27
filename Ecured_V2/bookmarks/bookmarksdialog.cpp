
#include "bookmarksdialog.h"
#include "bookmarknode.h"
#include "bookmarksmanager.h"
#include "bookmarksmodel.h"
#include "treesortfilterproxymodel.h"
#include <qheaderview.h>
#include <qmenu.h>
#include <QDirModel>

BookmarksDialog::BookmarksDialog(QWidget *parent, BookmarksManager *manager)
    : QDialog(parent), m_bookmarksManager(0) , m_bookmarksModel(0)    , m_proxyModel(0)
{
    m_bookmarksManager = manager;
    setupUi(this);

    tree->setUniformRowHeights(true);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setTextElideMode(Qt::ElideMiddle);

// Configuración del Idioma--(Español)
    QList<QAbstractButton *>botones = buttonBox->buttons();
    botones.at(0)->setText("&Cerrar");

    m_bookmarksModel = m_bookmarksManager->bookmarksModel();
    m_proxyModel = new TreeSortFilterProxyModel(this);
    m_proxyModel->setFilterKeyColumn(-1);

    connect(search, SIGNAL(textChanged(QString)),
            m_proxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()), tree, SLOT(removeSelected()));

    m_proxyModel->setSourceModel(m_bookmarksModel);

    // estableciendo el indice del tree en la raiz del marcador ->
    tree->setModel(m_proxyModel);
    tree->setCurrentIndex(m_proxyModel->index(0, 0));

    tree->setDragDropMode(QAbstractItemView::InternalMove);
    tree->setExpanded(m_proxyModel->index(0, 0), true);
    tree->setAlternatingRowColors(true);

    QFontMetrics fm(font());
    int header = fm.width(QLatin1Char('m')) * 40;
    tree->header()->resizeSection(0, header);
    tree->header()->setStretchLastSection(true);
    connect(tree, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(openBookmark()));
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenuRequested(const QPoint &)));
    connect(addFolderButton, SIGNAL(clicked()),
            this, SLOT(newFolder()));
    expandNodes(m_bookmarksManager->bookmarks());

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

BookmarksDialog::~BookmarksDialog()
{
    if (saveExpandedNodes(tree->rootIndex()))
        m_bookmarksManager->changeExpanded();
}

bool BookmarksDialog::saveExpandedNodes(const QModelIndex &parent)
{
    bool changed = false;
    for (int i = 0; i < m_proxyModel->rowCount(parent); ++i) {
        QModelIndex child = m_proxyModel->index(i, 0, parent);
        QModelIndex sourceIndex = m_proxyModel->mapToSource(child);
        BookmarkNode *childNode = m_bookmarksModel->node(sourceIndex);
        bool wasExpanded = childNode->expanded;
        if (tree->isExpanded(child)) {
            childNode->expanded = true;
            changed |= saveExpandedNodes(child);
        } else {
            childNode->expanded = false;
        }
        changed |= (wasExpanded != childNode->expanded);
    }
    return changed;
}

void BookmarksDialog::expandNodes(BookmarkNode *node)
{
    for (int i = 0; i < node->children().count(); ++i) {
        BookmarkNode *childNode = node->children()[i];
        if (childNode->expanded) {
            QModelIndex idx = m_bookmarksModel->index(childNode);
            idx = m_proxyModel->mapFromSource(idx);
            tree->setExpanded(idx, true);
            expandNodes(childNode);
        }
    }
}

void BookmarksDialog::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    QModelIndex index = tree->indexAt(pos);
    index = index.sibling(index.row(), 0);
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    const BookmarkNode *node = m_bookmarksModel->node(sourceIndex);
    /*if (index.isValid() && node->type() != BookmarkNode::Folder) {
        menu.addAction(tr("Abrir"), this, SLOT(openInCurrentTab()));
        menu.addAction(tr("Abrir en una nueva pestaña"), this, SLOT(openInNewTab()));
        menu.addSeparator();
    }
    menu.addSeparator();*/
    QAction *renameAction = menu.addAction("Editar Nombre", this, SLOT(editName()));
    renameAction->setEnabled(index.flags() & Qt::ItemIsEditable);
    if (index.isValid() && node->type() != BookmarkNode::Folder) {
        menu.addAction("Editar ", this, SLOT(editAddress()));
    }
    menu.addSeparator();
    QAction *deleteAction = menu.addAction(tr("Eliminar"), tree, SLOT(removeSelected()));
    deleteAction->setEnabled(index.flags() & Qt::ItemIsDragEnabled);
    menu.exec(QCursor::pos());
}

void BookmarksDialog::openBookmark(OpenUrlIn tab)
{
    QModelIndex index = tree->currentIndex();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    const BookmarkNode *node = m_bookmarksModel->node(sourceIndex);
    if (!index.parent().isValid() || !node || node->type() == BookmarkNode::Folder)
        return;
    emit openUrl(
          index.sibling(index.row(), 1).data(BookmarksModel::UrlRole).toUrl(),
          tab,
          index.sibling(index.row(), 0).data(Qt::DisplayRole).toString());
}

void BookmarksDialog::openBookmark()
{   
    openBookmark(UserOrCurrent);
}

void BookmarksDialog::openInCurrentTab()
{
    openBookmark(CurrentTab);
}

void BookmarksDialog::openInNewTab()
{
    openBookmark(NewTab);
}

void BookmarksDialog::editName()
{
    QModelIndex idx = tree->currentIndex();
    idx = idx.sibling(idx.row(), 0);
    tree->edit(idx);
}

void BookmarksDialog::editAddress()
{
    QModelIndex idx = tree->currentIndex();
    idx = idx.sibling(idx.row(), 1);
    tree->edit(idx);
}

void BookmarksDialog::newFolder()
{
    QModelIndex currentIndex = tree->currentIndex();
    QModelIndex idx = currentIndex;

    QModelIndex sourceIndex = m_proxyModel->mapToSource(idx);
    const BookmarkNode *sourceNode = m_bookmarksModel->node(sourceIndex);
    int row = -1; // default: append new folder as last item in selected folder
    if (sourceNode && sourceNode->type() != BookmarkNode::Folder) {
        // if selected item is not a folder, add new folder to parent folder,
        // but direcly below the selected item
        idx = idx.parent();
        row = currentIndex.row() + 1;
    }


    if (!idx.isValid())
        idx = m_proxyModel->index(1, 0); // Select Bookmarks menu as default
    idx = m_proxyModel->mapToSource(idx);


    BookmarkNode *parent = m_bookmarksManager->bookmarksModel()->node(idx);
    BookmarkNode *node = new BookmarkNode(BookmarkNode::Folder);
    node->title = "Nueva carpeta";
    m_bookmarksManager->addBookmark(parent, node, row);
}

