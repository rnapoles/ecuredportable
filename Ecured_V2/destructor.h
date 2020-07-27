#ifndef DESTRUCTOR_H
#define DESTRUCTOR_H

#include <QObject>
#include "mainwindow.h"
#include "searchengine.h"
#include "dbinfo.h"
#include "clucenehelper.h"

class Destructor : public QObject
{
    Q_OBJECT
public:
    explicit Destructor(QObject *parent = 0);

signals:

public slots:
    void destructor(){
        delete SearchEngine::instance();
        delete CLuceneHelper::getInstance();
        delete EcuredMainWindow::instance();
    }
};

#endif // DESTRUCTOR_H
