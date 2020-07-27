#ifndef ECUREDANNOUNCER_H
#define ECUREDANNOUNCER_H

#include <QThread>
#include "databasemanager.h"

class EcuredAnnouncer : public QThread
{
    Q_OBJECT
public:
    explicit EcuredAnnouncer(QObject *parent = 0);
    
private:
    DatabaseInfo dbinfo;
    void run();
    
public slots:
    
};

#endif // ECUREDANNOUNCER_H
