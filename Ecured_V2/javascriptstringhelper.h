#ifndef JAVASCRIPTSTRINGHELPER_H
#define JAVASCRIPTSTRINGHELPER_H

#include <QObject>
//#include <stdarg.h>
//#include <stdio.h>


class JavaScriptStringHelper : public QObject , public QString
{
    Q_OBJECT
public:
    explicit JavaScriptStringHelper(QObject *parent = 0);

signals:

public slots:
    //QString format(const char *cformat, ...);

};

#endif // JAVASCRIPTSTRINGHELPER_H
