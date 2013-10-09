TEMPLATE = lib
TARGET = core
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -L/home/michael/QtWin64/lib
        win32:QMAKE_LFLAGS += -shared
QMAKE_LFLAGS += -static-libgcc -static-libstdc++
# Input
SOURCES  +=
HEADERS += configdata.h \
           datapacketdecoder.h \
           datatype.h \
	   emscomms.h \
           lookupmetadata.h \
           memorylocationinfo.h \
           memorymetadata.h \
           readonlyramblock.h \
           readonlyramdata.h \
           serialportstatus.h \
	   tabledata.h \
           table2ddata.h \
           table2dmetadata.h \
           table3ddata.h \
	   table3dmetadata.h \
    rawdata.h \
    configblock.h \
    dialogfield.h \
    menuitem.h \
    submenuitem.h \
    menusetup.h \
    dialogitem.h
