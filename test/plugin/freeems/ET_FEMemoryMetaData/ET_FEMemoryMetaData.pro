#-------------------------------------------------
#
# Project created by QtCreator 2013-07-25T22:57:40
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ET_FEMemoryMetaData
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin
else:unix: LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin -lqjson

INCLUDEPATH += $$PWD/../../../../plugins/freeems \
               $$PWD/../../../../lib/core
DEPENDPATH  += $$PWD/../../../../plugins/freeems

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/freeemsplugin.dll
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/libfreeemsplugin.so

SOURCES += ET_FEMemoryMetaData.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
