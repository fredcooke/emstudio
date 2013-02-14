#EMStudio

Written by Michael Carpenter (malcom2073@gmail.com), and licensed under GPLv2.

### Overview:

I suppose I should have some real documentation here on how to use the different features of EMStudio....

### Binaries:

I maintain Windows and Linux 32bit binaries of both the master and dev branches on my website: http://mikesshop.net/EMStudio/

### Compiling:

#### Linux:

A few packages are needed to compile EMStudio. In Ubuntu:
$ sudo apt-get update
$ sudo apt-get install build-essential libqt4-dev libqt4-opengl-dev libqt4-opengl-dev qt4-qmake libqwt-dev libqt4-declarative libqjson-dev

Once this has completed (or the equivalant packages on your own distro), you can then clone the repository and build it. 

It has been mentioned before to run qmake -project, NEVER EVER EVER DO THIS!. This will break things.

To checkout and build:

$ git clone https://github.com/malcom2073/emstudio.git
$ cd emstudio
$ git checkout origin/BRANCHTOBUILD (typically dev or master)
$ qmake
$ make
$ ./emstudio


#### Windows:

I really can't support compiling on Windows. It's too much of a hassle and there are way too many variables and different
machine configurations to do any real support. If someone is interested in maintaining Windows binaries let me know and I'll
work through it with you, but it's too much of a mess to provide general support. I'll handle compiling binaries for now.


##More Information

http://malcom2073.github.com/emstudio/
