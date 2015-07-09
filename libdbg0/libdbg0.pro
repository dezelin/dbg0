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
    dwarfdie.cpp \
    dwarfattribute.cpp \
    dwarfdiefactory.cpp \
    dwarfattributefactory.cpp \
    dwarfaddressform.cpp \
    dwarfform.cpp \
    dwarfblockform.cpp \
    dwarfconstantform.cpp \
    dwarfexpressionlocform.cpp \
    dwarfflagform.cpp \
    dwarflineptrform.cpp \
    dwarflocationlistptrform.cpp \
    dwarfmacroptrform.cpp \
    dwarfrangelistptrform.cpp \
    dwarfreferenceform.cpp \
    dwarfstringform.cpp \
    dwarfformfactory.cpp

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
    attribute.h \
    dwarfattribute.h \
    dwarfdiefactory.h \
    dwarfattributefactory.h \
    form.h \
    dwarfaddressform.h \
    dwarfform.h \
    dwarfblockform.h \
    dwarfconstantform.h \
    dwarfexpressionlocform.h \
    dwarfflagform.h \
    dwarflineptrform.h \
    dwarflocationlistptrform.h \
    dwarfmacroptrform.h \
    dwarfrangelistptrform.h \
    dwarfreferenceform.h \
    dwarfstringform.h \
    dwarfformfactory.h \
    logger.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix|win32: LIBS += -ldwarf
unix|win32: LIBS += -lelf
