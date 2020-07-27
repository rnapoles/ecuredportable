#ifndef ECUREDSPLASHSCREEN_H
#define ECUREDSPLASHSCREEN_H

#include <QSplashScreen>
#include <QTimer>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

class EcuredSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    explicit EcuredSplashScreen(const QPixmap &pixmap = QPixmap());

signals:

public slots:

protected:
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void paintEvent ( QPaintEvent * event ) ;

private:
    QTimer *fadeTimer;
    QPoint  lastMousePosition;

signals:
    void opaque();

private slots:
    void fadeOut();
    void fadeTicker();

};

#endif // ECUREDSPLASHSCREEN_H
