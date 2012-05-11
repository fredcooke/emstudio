#-------------------------------------------------
#
# Project created by QtCreator 2012-05-03T11:03:53
#
#-------------------------------------------------

QT       += core gui declarative
CONFIG += debug
TARGET = freetune
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    freeemspacket.cpp \
    datafield.cpp \
    logloader.cpp \
    freeemscomms.cpp \
    gaugewidget.cpp \
    gaugeitem.cpp \
    serialthread.cpp \
    datapacketdecoder.cpp

HEADERS  += mainwindow.h \
    freeemspacket.h \
    datafield.h \
    logloader.h \
    freeemscomms.h \
    gaugewidget.h \
    gaugeitem.h \
    serialthread.h \
    datapacketdecoder.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.md
