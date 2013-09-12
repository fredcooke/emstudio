TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += lib     \
            plugins \
            #test \ Test is disabled for now, cross compiling with it does not work
            core

core.depends   += lib
plugins.depend += lib
test.depends   += lib     \
                  plugins
