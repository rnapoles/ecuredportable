#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QContextMenuEvent>
#include <QNetworkAccessManager>
#include "accessmanager/ecurednetworkaccessmanager.h"

//class EcuredNetworkAccessManager;

class EcuredWebView : public QWebView
{
    Q_OBJECT
public:
    explicit EcuredWebView(QWidget *parent = 0,QWidget *InspectorParent=0);

signals:

public slots:
    void openLinkInNewTab();
    void CopyLinkToClipboard();
    void CopyToClipboard();
    void InspectElement();

    void addbookmark();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void applyZoom();


protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
     QString TextLink;
     EcuredNetworkAccessManager *AccessManager;
     //QNetworkAccessManager *OldAccessManager;

     int m_currentZoom;
     QList<int> m_zoomLevels;
     int levelForZoom(int zoom);

};

#endif // WEBVIEW_H
