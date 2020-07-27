#ifndef WIKIHELPEER_H
#define WIKIHELPEER_H

#include <QObject>

class WikiHelper : public QObject
{
    Q_OBJECT
public:
    explicit WikiHelper(QObject *parent = 0);
    ~WikiHelper();
    void setTextContainer(const QString &cad);


private:
    QString TextContainer;
signals:

public slots:
    QString getTextContainer();
    QString convertToBase64(const QString &str);
   // void debug(const QString &str);
};

#endif // WIKIHELPEER_H
