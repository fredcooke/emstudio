#-------------------------------------------------
#
# Project created by QtCreator 2012-05-03T11:03:53
#
#-------------------------------------------------

QT       += core gui declarative
CONFIG += debug
TARGET = EMStudio
TEMPLATE = app
INCLUDEPATH += src

SOURCES += src/main.cpp\
	src/mainwindow.cpp \
    src/freeemspacket.cpp \
    src/datafield.cpp \
    src/logloader.cpp \
    src/freeemscomms.cpp \
    src/gaugewidget.cpp \
    src/gaugeitem.cpp \
    src/serialthread.cpp \
    src/datapacketdecoder.cpp \
    src/comsettings.cpp \
    src/rawdatadisplay.cpp \
    src/qhexedit.cpp \
    src/qhexedit_p.cpp \
    src/xbytearray.cpp \
    src/commands.cpp \
    src/rawdataview.cpp \
    src/gaugeview.cpp \
    src/emsinfoview.cpp \
    src/flagview.cpp \
    src/tableview.cpp \
    src/tableview2d.cpp \
    src/table2ddata.cpp


HEADERS  += src/mainwindow.h \
    src/freeemspacket.h \
    src/datafield.h \
    src/logloader.h \
    src/freeemscomms.h \
    src/gaugewidget.h \
    src/gaugeitem.h \
    src/serialthread.h \
    src/datapacketdecoder.h \
    src/comsettings.h \
    src/rawdatadisplay.h \
    src/qhexedit.h \
    src/qhexedit_p.h \
    src/xbytearray.h \
    src/commands.h \
    src/rawdataview.h \
    src/gaugeview.h \
    src/emsinfoview.h \
    src/flagview.h \
    src/tableview.h \
    src/tableview2d.h \
    src/table2ddata.h

FORMS    += src/mainwindow.ui \
    src/comsettings.ui \
    src/emsinfo.ui \
    src/datagauges.ui \
    src/datatables.ui \
    src/rawdatadisplay.ui \
    src/dataflags.ui \
    src/rawdataview.ui \
    src/tableview2d.ui

OTHER_FILES += \
    README.md
