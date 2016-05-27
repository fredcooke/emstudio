#EMStudio

Hacked and documented by Fred Cooke. Quality by original author. Licensed under GPLv2.

### Overview:

I suppose there should be some real documentation here on how to use the different features of EMStudio....

### Binaries:

No. Build from source, and Windows is not an option unless you're very keen. Several things flat out do not work on Windows. Building it on Windows is likely to be similarly painful to having toe nails removed with pliers.

### Compiling:

#### Linux:

A few packages are needed to compile EMStune. In Ubuntu:
$ sudo apt-get update
$ sudo apt-get install build-essential libqt4-dev libqt4-opengl-dev libqt4-opengl-dev qt4-qmake libqwt-dev libqt4-declarative libqjson-dev freeglut3-dev pkg-config

Once this has completed (or the equivalant packages on your own distro), you can then clone the repository and build it. 

It has been mentioned before to run qmake -project, NEVER EVER EVER DO THIS!. This will break things.

To checkout and build:

$ git clone https://github.com/fredcooke/emstudio.git
$ cd emstudio
$ git clone https://github.com/fredcooke/emstune-freeems.git plugins/freeems
$ ./checkout.sh origin/unstable
$ qmake -r
$ make
$ cd core
$ ./emstudio


#### Windows:

I really can't support compiling on Windows. It's too much of a hassle and there are way too many variables and different
machine configurations to do any real support. If someone is interested in maintaining Windows binaries let me know and I'll
work through it with you, but it's too much of a mess to provide general support. I'll handle compiling binaries for now.


##More Information

http://malcom2073.github.com/emstudio/
