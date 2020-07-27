
#include "searchbutton.h"

#include <qcompleter.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qpainter.h>

SearchButton::SearchButton(QWidget *parent)
    : QAbstractButton(parent)
    , m_showMenuTriangle(false)
{
    setFocusPolicy(Qt::NoFocus);
    setCursor(Qt::ArrowCursor);
    setMinimumSize(sizeHint());
}

QSize SearchButton::sizeHint() const
{
    if (!m_cache.isNull())
        return m_cache.size();
    if (m_showMenuTriangle)
        return QSize(16, 16);
    return QSize(12, 16);
}

QImage SearchButton::generateSearchImage(bool dropDown)
{
    QImage image(dropDown ? 16 : 12, 16, QImage::Format_ARGB32);
    image.fill(qRgba(0, 0, 0, 0));
    QPainterPath path;

    // draw magnify glass circle
    int radius = image.height() / 2;
    QRect circle(1, 1, radius, radius);
    path.addEllipse(circle);

    // draw handle
    path.arcMoveTo(circle, 300);
    QPointF currentPosition = path.currentPosition();
    path.moveTo(currentPosition.x() + 1, currentPosition.y() + 1);
    if (dropDown)
        path.lineTo(image.width()-6, image.height()-4);
    else
        path.lineTo(image.width()-2, image.height()-4);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.drawPath(path);

    if (dropDown) {
        // draw the dropdown triangle
        QPainterPath dropPath;
        dropPath.arcMoveTo(circle, 320);
        QPointF currentPosition = dropPath.currentPosition();
        currentPosition = QPointF(currentPosition.x() + 2, currentPosition.y() + 0.5);
        dropPath.moveTo(currentPosition);
        dropPath.lineTo(currentPosition.x() + 4, currentPosition.y());
        dropPath.lineTo(currentPosition.x() + 2, currentPosition.y() + 2);
        dropPath.closeSubpath();
        painter.setPen(Qt::darkGray);
        painter.setBrush(Qt::darkGray);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.drawPath(dropPath);
    }
    painter.end();
    return image;
}

void SearchButton::setImage(const QImage &image)
{
    m_cache = image;
    setMinimumSize(sizeHint());
    update();
}

void SearchButton::setShowMenuTriangle(bool show)
{
    m_showMenuTriangle = show;
    setMinimumSize(sizeHint());
}

bool SearchButton::showMenuTriangle() const
{
    return m_showMenuTriangle;
}

void SearchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (m_cache.isNull())
        m_cache = generateSearchImage(m_showMenuTriangle);
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), m_cache);
}

