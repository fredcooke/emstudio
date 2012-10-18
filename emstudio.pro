#-------------------------------------------------
#
# Project created by QtCreator 2012-05-03T11:03:53
#
#-------------------------------------------------

QT       += core gui declarative opengl
#CONFIG += debug
TARGET = emstudio
TEMPLATE = app
INCLUDEPATH += src
OBJECTS_DIR = obj
MOC_DIR = obj
UI_DIR = obj
QMAKE_LFLAGS += -static-libgcc
win32-x-g++ {
        message("Building for win32-x-g++")
	INCLUDEPATH += /home/michael/QtWin/libs/qwt/include /home/michael/QtWin/libs/qjson/include
	LIBS += -L/home/michael/QtWin/libs/qwt/lib -lqwt -L/home/michael/QtWin/libs/qjson/lib -lqjson0
        DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
        DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)
}
win32-g++ {
        message("Building for win32-g++")
        INCLUDEPATH += C:/libs/qwt/include C:/libs/qjson/include
        LIBS += -LC:/libs/qwt/lib -lqwt -LC:/libs/qjson/lib -lqjson0
        DEFINES += GIT_COMMIT=$$system(\"c:/program files/git/bin/git.exe\" describe --dirty=-DEV --always)
        DEFINES += GIT_HASH=$$system(\"c:/program files/git/bin/git.exe\" log -n 1 --pretty=format:%H)
}
unix {
	QMAKE_CXXFLAGS += -Werror
	target.path = /usr/bin
	config.path = /etc/emstudio
	config.files += freeems.config.json src/gauges.qml
	INSTALLS += target config
	LIBS += -lqwt -lqjson -lGL -lGLU -lglut
	INCLUDEPATH += /usr/include/qwt
	DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
	DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)
}
mac {
	QMAKE_CXXFLAGS += -Werror
	INCLUDEPATH += /opt/local/include
	INCLUDEPATH += /opt/local/include/qwt
}
SOURCES += src/main.cpp\
	src/mainwindow.cpp \
    src/freeemspacket.cpp \
    src/datafield.cpp \
    src/logloader.cpp \
    src/freeemscomms.cpp \
    src/gaugewidget.cpp \
    src/gaugeitem.cpp \
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
    src/table2ddata.cpp \
    src/packetstatusview.cpp \
    src/aboutview.cpp \
    src/memorylocation.cpp \
    src/tableview3d.cpp \
    src/interrogateprogressview.cpp \
    src/table3ddata.cpp \
    src/tabledata.cpp \
    src/readonlyramview.cpp \
    src/overviewprogressitemdelegate.cpp \
    src/dataview.cpp \
    src/emsstatus.cpp \
    src/tablemap3d.cpp \
    src/serialrxthread.cpp \
    src/tablewidget.cpp \
    src/serialport.cpp


HEADERS  += src/mainwindow.h \
    src/freeemspacket.h \
    src/datafield.h \
    src/logloader.h \
    src/freeemscomms.h \
    src/gaugewidget.h \
    src/gaugeitem.h \
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
    src/table2ddata.h \
    src/packetstatusview.h \
    src/aboutview.h \
    src/memorylocation.h \
    src/tableview3d.h \
    src/interrogateprogressview.h \
    src/table3ddata.h \
    src/headers.h \
    src/tabledata.h \
    src/readonlyramview.h \
    src/overviewprogressitemdelegate.h \
    src/dataview.h \
    src/emsstatus.h \
    src/tablemap3d.h \
    src/serialrxthread.h \
    src/tablewidget.h \
    src/serialport.h

FORMS    += src/mainwindow.ui \
    src/comsettings.ui \
    src/emsinfo.ui \
    src/datagauges.ui \
    src/datatables.ui \
    src/rawdatadisplay.ui \
    src/dataflags.ui \
    src/rawdataview.ui \
    src/tableview2d.ui \
    src/packetstatusview.ui \
    src/aboutview.ui \
    src/tableview3d.ui \
    src/interrogateprogressview.ui \
    src/readonlyramview.ui \
    src/emsstatus.ui

OTHER_FILES += \
    README.md
