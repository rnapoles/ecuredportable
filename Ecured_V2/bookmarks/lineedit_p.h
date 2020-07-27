#ifndef LINEEDIT_P_H
#define LINEEDIT_P_H

#include <qwidget.h>

class SideWidget : public QWidget
{
    Q_OBJECT

signals:
    void sizeHintChanged();

public:
    SideWidget(QWidget *parent = 0);

protected:
    bool event(QEvent *event);

};

#endif // LINEEDIT_P_H

