#-------------------------------------------------
#
# Project created by QtCreator 2012-05-03T11:03:53
#
#-------------------------------------------------

QT       += core gui declarative
CONFIG += debug
TARGET = EMStudio
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
    rawdatadisplay.cpp \
    qhexedit.cpp \
    qhexedit_p.cpp \
    xbytearray.cpp \
    commands.cpp \
    rawdataview.cpp \
    gaugeview.cpp \
    emsinfoview.cpp \
    flagview.cpp \
    tableview.cpp \
    tableview2d.cpp \
    table2ddata.cpp


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
    rawdatadisplay.h \
    qhexedit.h \
    qhexedit_p.h \
    xbytearray.h \
    commands.h \
    rawdataview.h \
    gaugeview.h \
    emsinfoview.h \
    flagview.h \
    tableview.h \
    tableview2d.h \
    table2ddata.h

FORMS    += mainwindow.ui \
    comsettings.ui \
    emsinfo.ui \
    datagauges.ui \
    datatables.ui \
    rawdatadisplay.ui \
    dataflags.ui \
    rawdataview.ui \
    tableview2d.ui

OTHER_FILES += \
    README.md
