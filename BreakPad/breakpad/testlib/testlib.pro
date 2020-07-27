#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T09:10:16
#
#-------------------------------------------------

QT       += core gui

TARGET = testlib
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/styles

SOURCES += styleplugin.cpp

HEADERS += styleplugin.h
symbian {
# Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE45FB951
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    pluginDeploy.sources = testlib.dll
    pluginDeploy.path = $$QT_PLUGINS_BASE_DIR/testlib
    DEPLOYMENT += pluginDeploy
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
