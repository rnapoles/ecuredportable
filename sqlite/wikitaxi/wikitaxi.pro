TARGET	 = wikitaxidb
#'SQLITE_FILE_HEADER=\"WikiTaxi DB 1.1\"'
DEFINES += SQLITE_ENABLE_FTS3 SQLITE_ENABLE_FTS3_PARENTHESIS
HEADERS		= ../../../sql/drivers/sqlite/qsql_sqlite.h \
    sqlite3.h
SOURCES		= smain.cpp \
		  ../../../sql/drivers/sqlite/qsql_sqlite.cpp \
    sqlite3.c

!system-sqlite:!contains( LIBS, .*sqlite.* ) {
    CONFIG(release, debug|release):DEFINES *= NDEBUG
    DEFINES += SQLITE_OMIT_LOAD_EXTENSION SQLITE_OMIT_COMPLETE 
    INCLUDEPATH += ../../../3rdparty/sqlite
    SOURCES +=
} else {
    LIBS *= $$QT_LFLAGS_SQLITE
    QMAKE_CXXFLAGS *= $$QT_CFLAGS_SQLITE
}

wince*: DEFINES += HAVE_LOCALTIME_S=0

include(../qsqldriverbase.pri)

#QMAKE_CXXFLAGS += -D
