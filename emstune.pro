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

#Ensure the pro file gets touched at the end of the build, so the next build reruns qmake
gittouch.commands = touch emstudio.pro core/core.pro plugins/freeems/freeems.pro
QMAKE_EXTRA_TARGETS += gittouch
POST_TARGETDEPS += gittouch
