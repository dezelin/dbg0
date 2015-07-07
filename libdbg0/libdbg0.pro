#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T01:09:49
#
#-------------------------------------------------

QT       -= core gui

TARGET = dbg0
TEMPLATE = lib

CONFIG   += c++11

DEFINES += LIBDBG0_LIBRARY

SOURCES += libdbg0.cpp \
    elfexecutable.cpp \
    dwarfsymboltable.cpp \
    dwarfcompilationunit.cpp \
    dwarfdie.cpp

HEADERS += libdbg0.h \
    cpu.h \
    executable.h \
    library.h \
    symboltable.h \
    symbolfile.h \
    elfexecutable.h \
    compilationunit.h \
    die.h \
    dwarfsymboltable.h \
    dwarfcompilationunit.h \
    dwarfdie.h \
    attribute.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix|win32: LIBS += -ldwarf
unix|win32: LIBS += -lelf
