#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T01:09:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dbg0
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libdbg0/release/ -ldbg0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libdbg0/debug/ -ldbg0
else:unix: LIBS += -L$$OUT_PWD/../libdbg0/ -ldbg0

INCLUDEPATH += $$PWD/../libdbg0
DEPENDPATH += $$PWD/../libdbg0
