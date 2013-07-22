TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += lib     \
            plugins \
            test    \
            core

core.depends = lib
test.depends = lib
