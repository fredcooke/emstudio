#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T00:04:00
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ET_SerialRXThread
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin
else:unix: LIBS += -L$$OUT_PWD/../../../../core/plugins/ -lfreeemsplugin

INCLUDEPATH += $$PWD/../../../../plugins/freeems \
               $$PWD/../../../../lib/core
DEPENDPATH  += $$PWD/../../../../plugins/freeems

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/freeemsplugin.dll
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../core/plugins/libfreeemsplugin.so

SOURCES += ET_SerialRXThread.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
