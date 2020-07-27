#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <zlib.h>

class Util
{
public:
    Util();
    static QString ucFirst(const QString &str);

};

QByteArray gUncompress(const QByteArray &data,bool &error);

#endif // UTIL_H
