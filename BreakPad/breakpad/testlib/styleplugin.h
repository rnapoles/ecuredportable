#ifndef STYLEPLUGIN_H
#define STYLEPLUGIN_H

#include <QtGui/QStylePlugin>


class StylePlugin : public QStylePlugin {
    Q_OBJECT
public:
    StylePlugin(QObject *parent = 0);
};

#endif // STYLEPLUGIN_H
