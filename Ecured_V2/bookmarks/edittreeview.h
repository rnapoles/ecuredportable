#ifndef EDITTREEVIEW_H
#define EDITTREEVIEW_H

#include <qtreeview.h>

class EditTreeView : public QTreeView
{
    Q_OBJECT

public:
    EditTreeView(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void removeSelected();
    void removeAll();

};

#endif // EDITTREEVIEW_H

