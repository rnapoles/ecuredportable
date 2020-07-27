#ifndef SEARCHBUTTON_H
#define SEARCHBUTTON_H

#include <qabstractbutton.h>

class QCompleter;
class SearchButton : public QAbstractButton
{
    Q_OBJECT

public:
    SearchButton(QWidget *parent = 0);
    void setImage(const QImage &image);
    void setShowMenuTriangle(bool show);
    bool showMenuTriangle() const;
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;

private:
    QImage generateSearchImage(bool dropDown);
    QImage m_cache;
    bool m_showMenuTriangle;
};

#endif // SEARCHBUTTON_H

