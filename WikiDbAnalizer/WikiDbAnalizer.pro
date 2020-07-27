#-------------------------------------------------
#
# Project created by QtCreator 2013-02-02T15:46:24
#
#-------------------------------------------------

QT       += core gui sql

TARGET = WikiDbAnalizer
TEMPLATE = app

INCLUDEPATH += $$PWD/Ecured
INCLUDEPATH += $$PWD/zlib

 win32 {
  INCLUDEPATH += $$PWD/includes/Windows
  LIBS += -L $$PWD/libs/Windows -lzlib
  LIBS += -lbreakpad

 }

 unix {
  INCLUDEPATH += $$PWD/includes/Unix
  LIBS += -L $$PWD/libs/Unix -lz -lbz2 -lbreakpad
 }

SOURCES += main.cpp\
        mainwindow.cpp \
    wiki/dbinfo.cpp \
    wiki/databasemanager.cpp \
    ecured/qtiocompressor.cpp

HEADERS  += mainwindow.h \
    wiki/databasemanager.h \
    wiki/dbinfo.h \
    ecured/CommonDefines.h \
    ecured/qtiocompressor.h

FORMS    += mainwindow.ui

RESOURCES += \
    recursos.qrc

RC_FILE = icon.rc
