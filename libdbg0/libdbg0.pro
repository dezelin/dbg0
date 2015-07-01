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

HEADERS += libdbg0.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
