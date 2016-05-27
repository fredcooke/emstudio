#EMStudio

Hacked and lightly documented by Fred Cooke. Quality (or lack thereof) by original author. Licensed under GPLv2.

### Overview:

EMStudio was thrown together in an impressively short time frame and has all of the maintenance and quality issues associated with proof-of-concept development style. Right now, it's your only choice for live tuning FreeEMS. Sorry.

### Binaries:

No. Build from source, and Windows is not an option unless you're very keen. Several things flat out do not work on Windows. Building it on Windows is likely to be similarly painful to having toe nails removed with pliers.

### Compiling:

#### Linux:

A few packages are needed to compile EMStudio. In Debian/Ubuntu/Mint:

* sudo apt-get update
* sudo apt-get install build-essential libqt4-dev libqt4-opengl-dev libqt4-opengl-dev qt4-qmake libqwt-dev libqt4-declarative libqjson-dev freeglut3-dev pkg-config

Once this has completed (or the equivalant packages on your own distro), you can then clone the repository and build it. 

It has been mentioned before to run qmake -project, NEVER EVER EVER DO THIS!. This will break things.

To checkout and build:

* git clone https://github.com/fredcooke/emstudio.git
* cd emstudio
* git clone https://github.com/fredcooke/emstune-freeems.git plugins/freeems
* # Not currently necessary: ./checkout.sh origin/unstable
* qmake -r
* make
* cd core # yuck
* ./emstudio


#### Windows:

Unsupported operating system. EMStudio's replacement will function flawlessly on Windows, but is currently vapourware.

##More Information

http://forum.diyefi.org

