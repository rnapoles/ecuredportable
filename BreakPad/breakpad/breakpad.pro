#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T14:09:42
#
#-------------------------------------------------

CONFIG += warn_on thread exceptions rtti stl
QT       -= gui
#QT       += core

TARGET = breakpad
TEMPLATE = lib

DEFINES += BREAKPAD_LIBRARY
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII

SOURCES += \
    BreakpadHandler.cpp

HEADERS += \
    BreakpadHandler.h


unix:!mac {
        debug {
                # google-breakpad supports only stabs symbols on GNU/Linux and *BSD for now
                QMAKE_CFLAGS_DEBUG +=-gstabs
                QMAKE_CXXFLAGS_DEBUG +=-gstabs
        }
}

BREAKPAD_PATH = $$PWD/third-party/google-breakpad/src
INCLUDEPATH += $$BREAKPAD_PATH

# every *nix
unix {
        SOURCES += $$BREAKPAD_PATH/client/minidump_file_writer.cc \
                $$BREAKPAD_PATH/common/string_conversion.cc \
                $$BREAKPAD_PATH/common/convert_UTF.c \
                $$BREAKPAD_PATH/common/md5.c
}

# mac os x
mac {
        # hack to make minidump_generator.cc compile as it uses
        # esp instead of __esp
        # DEFINES += __DARWIN_UNIX03=0 -- looks like we doesn't need it anymore

        SOURCES += $$BREAKPAD_PATH/client/mac/handler/exception_handler.cc \
                $$BREAKPAD_PATH/client/mac/handler/minidump_generator.cc \
                $$BREAKPAD_PATH/client/mac/handler/dynamic_images.cc \
                $$BREAKPAD_PATH/common/mac/string_utilities.cc \
                $$BREAKPAD_PATH/common/mac/file_id.cc \
                $$BREAKPAD_PATH/common/mac/macho_id.cc \
                $$BREAKPAD_PATH/common/mac/macho_utilities.cc \
                $$BREAKPAD_PATH/common/mac/macho_walker.cc
}

# other *nix
unix:!mac {
        SOURCES += $$BREAKPAD_PATH/client/linux/handler/exception_handler.cc \
                $$BREAKPAD_PATH/client/linux/handler/minidump_generator.cc \
                $$BREAKPAD_PATH/client/linux/handler/linux_thread.cc \
                $$BREAKPAD_PATH/common/linux/guid_creator.cc \
                $$BREAKPAD_PATH/common/linux/file_id.cc
}

win32 {
        SOURCES += $$BREAKPAD_PATH/client/windows/handler/exception_handler.cc \
                $$BREAKPAD_PATH/client/windows/crash_generation/crash_generation_client.cc \
                $$BREAKPAD_PATH/common/windows/guid_string.cc
}
