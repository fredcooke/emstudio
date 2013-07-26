#-------------------------------------------------
#
# Project created by QtCreator 2013-07-25T23:25:44
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ET_FETable3DData
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

SOURCES += ET_FETable3DData.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
