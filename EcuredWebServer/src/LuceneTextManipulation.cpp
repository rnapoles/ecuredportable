#include "LuceneTextManipulation.h"

TCHAR* QStringToTChar(const QString &str)
{
    TCHAR *string = new TCHAR[(str.length() +1) * sizeof(TCHAR)];
    memset(string, 0, (str.length() +1) * sizeof(TCHAR));
    str.toWCharArray(string);
    return string;
}

QString TCharToQString(const TCHAR *string)
{
    QString retValue = QString::fromWCharArray(string);
    return retValue;
}


