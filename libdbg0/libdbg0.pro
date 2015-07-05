#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T01:09:49
#
#-------------------------------------------------

QT       -= core gui

TARGET = dbg0
TEMPLATE = lib

DEFINES += LIBDBG0_LIBRARY

SOURCES += libdbg0.cpp

HEADERS += libdbg0.h \
    cpu.h \
    executable.h \
    symboltable.h \
    library.h \
    symbolfile.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


unix|win32: LIBS += -ldwarf
unix|win32: LIBS += -lelf
