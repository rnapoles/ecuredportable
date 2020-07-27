QT = gui core network sql script webkit
TARGET = EcuredWebServer
TEMPLATE = app
#CONFIG += CONSOLE
CONFIG += GUI

CONFIG(debug, debug|release) {
  DEFINES += SUPERVERBOSE
}

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/database
INCLUDEPATH += $$PWD/search
INCLUDEPATH += $$PWD/libs

 win32 {
  INCLUDEPATH += $$PWD/includes/Windows
  LIBS += -L $$PWD/libs/Windows -lzlib
#  LIBS += -lbreakpad
  LIBS += -lclucene-core -lclucene-shared -lclucene-contribs-lib
  LIBS += -lbz2
        system(del release/qrc_recursos.cpp)
        system(del debug/qrc_recursos.cpp)
 }

 unix {
  INCLUDEPATH += $$PWD/includes/Unix
  LIBS += -L $$PWD/libs/Unix
  LIBS += -lz
  LIBS += -lbz2
  LIBS += -lbreakpad
  LIBS += -lclucene-core -lclucene-shared -lclucene-contribs-lib
 }

HEADERS = \
    static.h \
    requestmapper.h \
    controller/dumpcontroller.h \
    controller/templatecontroller.h \
    controller/formcontroller.h \
    controller/fileuploadcontroller.h \
    controller/sessioncontroller.h \
    controller/staticfilecontroller.h \
    controller/indexcontroller.h \
    servicemanager.h \
    ../lib/qtsingleapplication/qtsinglecoreapplication.h \
    ../lib/qtsingleapplication/qtsingleapplication.h \
    ../lib/qtsingleapplication/QtSingleApplication \
    ../lib/qtsingleapplication/qtlockedfile.h \
    ../lib/qtsingleapplication/QtLockedFile \
    ../lib/qtsingleapplication/qtlocalpeer.h \
#    search/clucenehelper.h \
#    search/searchengine.h \
    database/dbquery.h \
    database/databasemanager.h \
    libs/qtiocompressor.h \
    libs/util.h \
    controller/javascriptcontroller.h \
    controller/lucenecontroller.h \
    search/lucenedocument.h \
    announcer/ecuredannouncer.h \
    clucenehelper.h \
    LuceneTextManipulation.h

SOURCES = main.cpp \
    static.cpp \
    requestmapper.cpp \
    controller/dumpcontroller.cpp \
    controller/templatecontroller.cpp \
    controller/formcontroller.cpp \
    controller/fileuploadcontroller.cpp \
    controller/sessioncontroller.cpp \
    controller/staticfilecontroller.cpp \
    controller/indexcontroller.cpp \
    servicemanager.cpp \
    ../lib/qtsingleapplication/qtsinglecoreapplication.cpp \
    ../lib/qtsingleapplication/qtsingleapplication.cpp \
    ../lib/qtsingleapplication/qtlockedfile_win.cpp \
    ../lib/qtsingleapplication/qtlockedfile_unix.cpp \
    ../lib/qtsingleapplication/qtlockedfile.cpp \
    ../lib/qtsingleapplication/qtlocalpeer.cpp \
#    search/searchengine.cpp \
#    search/clucenehelper.cpp \
    database/dbquery.cpp \
    database/databasemanager.cpp \
    libs/qtiocompressor.cpp \
    libs/util.cpp \
    controller/javascriptcontroller.cpp \
    controller/lucenecontroller.cpp \
    search/lucenedocument.cpp \
    announcer/ecuredannouncer.cpp \
    clucenehelper.cpp \
    LuceneTextManipulation.cpp

OTHER_FILES += \
    ../etc/EcuredWebServer.ini \
    ../etc/demo.tpl \
    ../etc/demo-de.tpl \
    ../doc/releasenotes.txt \
    ../etc/docroot/Schmetterling klein.png \
    ../etc/docroot/index.html \
    ../etc/docroot/Schmetterling klein.png \
    ../Doxyfile \
    ../etc/templates/demo-de.tpl \
    ../etc/templates/demo.tpl \
    ../doc/releasenotes.txt \
    ../doc/license.txt \
    ../doc/example-response-normal.txt \
    ../doc/example-response-chunked.txt \
    ../doc/example-request-get.txt \
    ../doc/example-request-form-post.txt \
    ../doc/example-request-form-get.txt \
    ../doc/example-request-file-upload.txt \
    ../doc/readme.txt

include(../lib/bfLogging/src/bfLogging.pri)
include(../lib/bfHttpServer/src/bfHttpServer.pri)
include(../lib/bfTemplateEngine/src/bfTemplateEngine.pri)
include(../lib/qjson/qjson.pri)
#include(../lib/qtsingleapplication/qtsingleapplication.pri)
#include(../lib/qtsingleapplication/qtsinglecoreapplication.pri)

RESOURCES += \
    resources.qrc
    
RC_FILE = icon.rc       
