
#ifndef TREESORTFILTERPROXYMODEL_H
#define TREESORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

class TreeSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TreeSortFilterProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // TREESORTFILTERPROXYMODEL_H

