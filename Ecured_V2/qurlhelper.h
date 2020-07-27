#ifndef QURLHELPER_H
#define QURLHELPER_H

#include <QUrl>

class QUrlHelper : public QUrl
{
public:
    QUrlHelper(const QString &url);
    QString obtenerParametro() const;
};

#endif // QURLHELPER_H
