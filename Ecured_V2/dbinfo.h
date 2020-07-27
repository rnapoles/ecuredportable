#ifndef DBINFO_H
#define DBINFO_H
#include <QtCore>
#include <QMutex>

class DbInfo
{
public:
    static DbInfo *getInstance()
    {
        if (s_instance==NULL){
            QMutex m;
            m.lock();
            s_instance = new DbInfo;
            m.unlock();
        }
        return s_instance;
    }

    QString getFecha() ;
    QString getCantidadArticulos() ;
    void updateInfo();

private:
    static DbInfo *s_instance;
    QString pfecha;
    QString pCantida_art;
    DbInfo();

};

#endif // DBINFO_H
