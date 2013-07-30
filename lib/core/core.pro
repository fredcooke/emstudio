QT       -= gui
TARGET    = core
TEMPLATE  = lib
CONFIG   += staticlib
SOURCES  += datafield.cpp        \
            tabledata.cpp
HEADERS  += datafield.h          \
            tabledata.h          \
            emscomms.h           \
            memorymetadata.h     \
            datapacketdecoder.h  \
            table2ddata.h        \
            table3ddata.h        \
            headers.h            \
            configdata.h         \
            readonlyramdata.h    \
            lookupmetadata.h     \
            readonlyramblock.h   \
            table3dmetadata.h    \
            table2dmetadata.h    \
            memorylocationinfo.h \
            datatype.h           \
            serialportstatus.h

