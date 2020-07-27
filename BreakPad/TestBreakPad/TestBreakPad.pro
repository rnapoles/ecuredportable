#-------------------------------------------------
#
# Project created by QtCreator 2012-12-12T17:10:13
#
#-------------------------------------------------

QT       += core gui

TARGET = TestBreakPad
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -L $$PWD
LIBS += -lbreakpad
