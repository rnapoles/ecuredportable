#-------------------------------------------------
#
# Project created by QtCreator 2011-03-26T08:50:21
#
#-------------------------------------------------

CONFIG += qt
CONFIG += debug_and_release
CONFIG += rtti

QT       += gui sql webkit network xml


TARGET = Ecured
TEMPLATE = app

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/includes/google-breakpad

     debug {

     }

    release {

    }

 win32 {
  INCLUDEPATH += $$PWD/includes/Windows
  LIBS += -L $$PWD/libs/Windows -lzlib
  LIBS += -lbreakpad
  LIBS += -lclucene-core.dll -lclucene-shared.dll -lclucene-contribs-lib.dll
  LIBS += -lbz2
    #CONFIG(release){
    #    system(del release/qrc_recursos.cpp)
    #    system(del debug/qrc_recursos.cpp)
    #    system(del release/qrc_recursos.o)
    #    system(del debug/qrc_recursos.o)
    #}


 }

 unix {
  INCLUDEPATH += $$PWD/includes/Unix
  LIBS += -L $$PWD/libs/Unix -lz -lbz2 -lbreakpad
  LIBS += -lclucene-core -lclucene-shared -lclucene-contribs-lib
 }

win32:QMAKE_RCC = $$[QT_INSTALL_BINS]\\rcc.exe

#3dnow	AMD 3DNow! instruction support is enabled.
#exceptions	Exception support is enabled.
#mmx	Intel MMX instruction support is enabled.
#rtti	RTTI support is enabled.
#stl	STL support is enabled.
#sse	SSE support is enabled.
#sse2	SSE2 support is enabled.


SOURCES += main.cpp\
        mainwindow.cpp \
    page.cpp \
    findwidget.cpp \
    webview.cpp \
    qurlhelper.cpp \
    javascriptstringhelper.cpp \
    querythread.cpp \
    searchengine.cpp \
    accessmanager/ecurednetworkaccessmanager.cpp \
    wikihelpeer.cpp \
    util.cpp \
    dialogacercade.cpp \
    import.cpp \
    destructor.cpp \
    clucenehelper.cpp \
    databasemanager.cpp \
    bookmarks/lineedit.cpp \
    bookmarks/bookmarksmodel.cpp \
    bookmarks/bookmarksmenu.cpp \
    bookmarks/bookmarksmanager.cpp \
    bookmarks/bookmarksdialog.cpp \
    bookmarks/bookmarknode.cpp \
    bookmarks/addbookmarkdialog.cpp \
    bookmarks/xbelwriter.cpp \
    bookmarks/xbelreader.cpp \
    bookmarks/modelmenu.cpp \
    bookmarks/edittreeview.cpp \
    bookmarks/treesortfilterproxymodel.cpp \
    bookmarks/autosaver.cpp \
    bookmarks/searchlineedit.cpp \
    bookmarks/searchbutton.cpp \
    bookmarks/clearbutton.cpp \
    indexgeneratorwizard.cpp \
    avancesearchdialog.cpp \
    qballoontip.cpp \
    ecuredsplashscreen.cpp \
    ImportSource/importdocumentalsourcewizard.cpp \
    qtiocompressor.cpp \
    qbzip2device.cpp \
    ImportSource/wikidbimporter.cpp \
    ImportSource/xmlbz2importer.cpp \
    mediawikixmlhandler.cpp \
    ImportSource/wikitaxiimporter.cpp \
    libs/qtsingleapplication/qtsinglecoreapplication.cpp \
    libs/qtsingleapplication/qtsingleapplication.cpp \
    libs/qtsingleapplication/qtlockedfile_win.cpp \
    libs/qtsingleapplication/qtlockedfile_unix.cpp \
    libs/qtsingleapplication/qtlockedfile.cpp \
    libs/qtsingleapplication/qtlocalpeer.cpp \
    ErrorReporter/reporter.cpp \
    dbquery.cpp \
    dialogecuredserverbrowser.cpp \
    lucenedocument.cpp \
    appconfig.cpp


HEADERS  += mainwindow.h \
    page.h \
    findwidget.h \
    webview.h \
    qurlhelper.h \
    javascriptstringhelper.h \
    searchengine.h \
    accessmanager/ecurednetworkaccessmanager.h \
    wikihelpeer.h \
    util.h \
    dialogacercade.h \
    import.h \
    destructor.h \
    clucenehelper.h \
    databasemanager.h \
    bookmarks/lineedit_p.h \
    bookmarks/lineedit.h \
    bookmarks/bookmarksmodel.h \
    bookmarks/bookmarksmenu.h \
    bookmarks/bookmarksmanager.h \
    bookmarks/bookmarksdialog.h \
    bookmarks/bookmarknode.h \
    bookmarks/addbookmarkdialog.h \
    bookmarks/xbelwriter.h \
    bookmarks/xbelreader.h \
    bookmarks/modelmenu.h \
    bookmarks/edittreeview.h \
    bookmarks/treesortfilterproxymodel.h \
    bookmarks/autosaver.h \
    bookmarks/searchlineedit.h \
    bookmarks/searchbutton.h \
    bookmarks/clearbutton.h \
    querythread.h \
    indexgeneratorwizard.h \
    avancesearchdialog.h \
    qballoontip.h \
    ecuredsplashscreen.h \
    singleton.h \
    ImportSource/importdocumentalsourcewizard.h \
    CommonDefines.h \
    qbzip2device.h \
    qtiocompressor.h \
    ImportSource/wikidbimporter.h \
    ImportSource/xmlbz2importer.h \
    mediawikixmlhandler.h \
    ImportSource/wikitaxiimporter.h \
    libs/qtsingleapplication/qtsinglecoreapplication.h \
    libs/qtsingleapplication/qtsingleapplication.h \
    libs/qtsingleapplication/QtSingleApplication \
    libs/qtsingleapplication/qtlockedfile.h \
    libs/qtsingleapplication/QtLockedFile \
    libs/qtsingleapplication/qtlocalpeer.h \
#    includes/google-breakpad/BreakpadHandler.h \
    ErrorReporter/reporter.h \
    dbquery.h \
    dialogecuredserverbrowser.h \
    lucenedocument.h \
    appconfig.h

include(./libs/qjson/qjson.pri)


FORMS    += mainwindow.ui \
    dialogacercade.ui \
    import.ui \
    bookmarks/bookmarksdialog.ui \
    bookmarks/addbookmarkdialog.ui \
    avancesearchdialog.ui \
    dialogecuredserverbrowser.ui

RESOURCES += \
    recursos.qrc
    
RC_FILE = icon.rc    

OTHER_FILES +=




