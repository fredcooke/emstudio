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
    datapacketdecoder.cpp \
    comsettings.cpp \
    emsinfo.cpp \
    datagauges.cpp \
    datatables.cpp \
    rawdatadisplay.cpp

HEADERS  += mainwindow.h \
    freeemspacket.h \
    datafield.h \
    logloader.h \
    freeemscomms.h \
    gaugewidget.h \
    gaugeitem.h \
    serialthread.h \
    datapacketdecoder.h \
    comsettings.h \
    emsinfo.h \
    datagauges.h \
    datatables.h \
    rawdatadisplay.h

FORMS    += mainwindow.ui \
    comsettings.ui \
    emsinfo.ui \
    datagauges.ui \
    datatables.ui \
    rawdatadisplay.ui

OTHER_FILES += \
    README.md
