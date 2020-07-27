
#include "edittreeview.h"
#include <qevent.h>

EditTreeView::EditTreeView(QWidget *parent)
    : QTreeView(parent)
{
    //this->setCurrentIndex(Q);
}

void EditTreeView::keyPressEvent(QKeyEvent *event)
{
    if (model() && event->key() == Qt::Key_Delete) {
        removeSelected();
        event->setAccepted(true);
    } else {
        QAbstractItemView::keyPressEvent(event);
    }
}

void EditTreeView::removeSelected()
{
    if (!model() || !selectionModel() || !selectionModel()->hasSelection())
        return;

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    for (int i = selectedRows.count() - 1; i >= 0; --i) {
        QModelIndex idx = selectedRows.at(i);
        model()->removeRow(idx.row(), idx.parent());
    }
}

void EditTreeView::removeAll()
{
    if (!model())
        return;

    model()->removeRows(0, model()->rowCount(rootIndex()), rootIndex());
}

