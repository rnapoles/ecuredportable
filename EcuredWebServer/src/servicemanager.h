#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include "httplistener.h"

class Servicemanager : public QObject
{
    Q_OBJECT
public:
    explicit Servicemanager(QObject *parent = 0);
    ~Servicemanager();

    
public slots:
    void StartApp();
    void StopApp();
    void handleMessage(const QString& message);

private:
    QAction *quitAction;
    QMenu *trayIconMenu;
    QSystemTrayIcon *trayIcon;
    HttpListener *httplistener;

};

#endif // SERVICEMANAGER_H
