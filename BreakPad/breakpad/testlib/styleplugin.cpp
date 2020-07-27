#include "styleplugin.h"


StylePlugin::StylePlugin(QObject *parent) :
    QStylePlugin(parent)
{
}

Q_EXPORT_PLUGIN2(testlib, StylePlugin)
