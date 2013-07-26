QT       -= gui
TARGET    = core
TEMPLATE  = lib
CONFIG   += staticlib
SOURCES  += datafield.cpp       \
            tabledata.cpp
HEADERS  += datafield.h         \
            tabledata.h         \
            emscomms.h          \
            memorymetadata.h    \
            datapacketdecoder.h \
            table2ddata.h       \
            table3ddata.h       \
            headers.h

