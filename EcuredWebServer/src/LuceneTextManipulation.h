#ifndef LUCENETEXTMANIPULATION_H
#define LUCENETEXTMANIPULATION_H
#include <QString>
#include <CLucene.h>

TCHAR* QStringToTChar(const QString &str);
QString TCharToQString(const TCHAR *string);
#endif // LUCENETEXTMANIPULATION_H

