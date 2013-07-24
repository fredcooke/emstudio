TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += lib     \
            plugins \
            test    \
            core

core.depends   += lib
plugins.depend += lib
test.depends   += lib     \
                  plugins
