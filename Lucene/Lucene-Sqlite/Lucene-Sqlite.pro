#-------------------------------------------------
#
# Project created by QtCreator 2012-01-13T10:12:51
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = Lucene-Sqlite
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD
LIBS += -L $$PWD -lzlib
LIBS += -lclucene-core.dll -lclucene-shared.dll -lclucene-contribs-lib.dll

SOURCES += main.cpp
