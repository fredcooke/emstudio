#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T22:51:42
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ET_FEDataPacketDecoder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin
else:unix: LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin -lqjson

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/freeemsplugin.dll
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/libfreeemsplugin.so

INCLUDEPATH += $$PWD/../../../../plugins/freeems \
               $$PWD/../../../../core/src
DEPENDPATH  += $$PWD/../../../../plugins/freeems \
               $$PWD/../../../../core/src

SOURCES += ET_FEDataPacketDecoder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
