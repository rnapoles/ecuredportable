#include "ecuredsplashscreen.h"

EcuredSplashScreen::EcuredSplashScreen(const QPixmap &pixmap) :
    QSplashScreen(pixmap)
{

    setAutoFillBackground(true);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setWindowOpacity(0.0);

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::ApplicationModal);

    this->fadeTimer = new QTimer(this);
    fadeTimer->setInterval(50);
    connect(fadeTimer,SIGNAL(timeout()),this,SLOT(fadeTicker()));
    fadeTimer->start();



}

void EcuredSplashScreen::mouseMoveEvent ( QMouseEvent * event ){

}

void EcuredSplashScreen::mousePressEvent ( QMouseEvent * event ){

}

void EcuredSplashScreen::paintEvent(QPaintEvent *event)
{
    /*QPainterPath path;
    path.cubicTo(rect().topLeft(), rect().bottomLeft(),
    rect().bottomRight());
    path.cubicTo(rect().topRight(), rect().bottomRight(),
    rect().bottomLeft());
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawTiledPixmap(rect(), QPixmap("qt.png"));
    p.setBrush(QColor::fromRgba(qRgba(0,0,0,128)));
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.drawPath(path);*/
}

void EcuredSplashScreen::fadeOut(){

}

void EcuredSplashScreen::fadeTicker(){
    qreal opacity = windowOpacity()+0.1;

    setWindowOpacity(opacity);

    if (opacity >= 1.1)
    {
        fadeTimer->stop();
        QTimer::singleShot(2000,this, SLOT(close()));
        emit opaque();
    }
}
