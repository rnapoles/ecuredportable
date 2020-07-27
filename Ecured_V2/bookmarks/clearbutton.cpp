
#include "clearbutton.h"

#include <qpainter.h>

ClearButton::ClearButton(QWidget *parent)
    : QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
    setToolTip(tr("Clear"));
    setMinimumSize(22, 22);
    setVisible(false);

#if QT_VERSION >= 0x040600
    // First check for a style icon, current KDE provides one
    if (m_styleImage.isNull()) {
        QLatin1String iconName = (layoutDirection() == Qt::RightToLeft)
            ? QLatin1String("edit-clear-locationbar-ltr")
            : QLatin1String("edit-clear-locationbar-rtl");
        QIcon icon = QIcon::fromTheme(iconName);
        if (!icon.isNull())
            m_styleImage = icon.pixmap(16, 16).toImage();
    }
#endif
}

void ClearButton::textChanged(const QString &text)
{
    setVisible(!text.isEmpty());
}

void ClearButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (!m_styleImage.isNull()) {
        int x = (width() - m_styleImage.width()) / 2 - 1;
        int y = (height() - m_styleImage.height()) / 2 - 1;
        painter.drawImage(x, y, m_styleImage);
        return;
    }

    // Fall back to boring circle X
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPalette p = palette();
    QColor circleColor = isDown() ? p.color(QPalette::Dark) : p.color(QPalette::Mid);
    QColor xColor = p.color(QPalette::Window);

    // draw circle
    painter.setBrush(circleColor);
    painter.setPen(circleColor);
    int padding = width() / 5;
    int circleRadius = width() - (padding * 2);
    painter.drawEllipse(padding, padding, circleRadius, circleRadius);

    // draw X
    painter.setPen(xColor);
    padding = padding * 2;
    painter.drawLine(padding, padding,            width() - padding, width() - padding);
    painter.drawLine(padding, height() - padding, width() - padding, padding);
}

