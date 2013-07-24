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

INCLUDEPATH += $$PWD/../../../../plugins/freeems \
               $$PWD/../../../../lib/core           # due to hdr dependency - fix in refactor phase
DEPENDPATH  += $$PWD/../../../../plugins/freeems

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/freeemsplugin.dll
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/libfreeemsplugin.so

SOURCES += ET_FEDataPacketDecoder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
