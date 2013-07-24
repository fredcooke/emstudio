#-------------------------------------------------
#
# Project created by QtCreator 2013-07-23T21:56:21
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ET_TableData
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../lib/core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../lib/core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../../../../lib/core/ -lcore

INCLUDEPATH += $$PWD/../../../../lib/core
DEPENDPATH += $$PWD/../../../../lib/core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/core/release/core.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/core/debug/core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/core/libcore.a

SOURCES += ET_TableData.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
