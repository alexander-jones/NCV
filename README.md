Neocortical View (NCV):
=======================

<p align="center">
<a href="http://youtu.be/hP85egv3eL8"><img src="http://i.imgur.com/bU2ojSH.png" alt="NCV Introduction Video"/></a>
</p>

Table of Contents:
------------------

* [Prerequisites](#prerequisites)
* [Installing Dependencies](#dependencies)
* [Building](#building)
* [Running](#running)
* [Developing Plugins](#plugins)

<a name="prerequisites"/>
Prerequisites:
-------------
* In order for the visualization plugin to render properly your host system must have a GPU with drivers capable of
creating a OpenGL 3.3 context
* [NCS 6](http://www.cse.unr.edu/brain/ncs6) and all it's dependencies must be installed on your host system.
* You must be running a Windows 7, Linux, or Mac OSX operating system.

<a name="dependencies"/>
Installing Dependencies:
---------------------

#### On Mac / OSX:

* Install the XCode Command Line Tools from the Downloads section of XCode.

* Install [Fink](http://www.finkproject.org/download/index.php?phpLang=en) access the `apt-get` command from the command line for installing dependencies.

* Follow the Linux guide below to install all dependencies.

#### On Linux / Unix:

Multiple libraries needed by NCV require CMake to generate makefiles, so if your system does not contain CMake
enter `sudo apt-get install cmake` into a terminal


* To install the Qt 4.8.4 Dependencies:

    * Download the Qt 4.8.4 archive from [here](http://qt-project.org/downloads), extract it, and navigate into the extracted directory with a terminal.
    * Enter `./configure` into the terminal to configure the installation & generate makefiles.
    * Enter `make` into the terminal to build Qt 4.8.4.
    * Enter `sudo make install` into the terminal to install Qt 4.8.4 into your system path.
    * Enter `sudo apt-get install libqxt-dev` into the terminal to install the libQxt extenstion library.

* To install the GLEW *9.0 (OpenGL Extension Wrangler) library :

    * Download the GLEW archive from [here](http://www.libssh.org/files/0.5/), extract it, and navigate into the extracted directory with a terminal.
    * Enter `cmake .` into the terminal to  configure the installation & generate makefiles.
    * Enter `make` into the terminal to build GLEW.
    * Enter `sudo make install.all` into the terminal to install GLEW into your system path.

* To install the libssh 0.5.0 library:

    * Download the libssh 0.5.0 archive from [here](http://www.libssh.org/files/0.5/), extract it, and navigate to the directory
    containing the extracted directory with a terminal.
    * Enter `sudo apt-get install libxmu-dev libxu-dev` into the terminal to download libssh's dependencies.
    * Enter `mkdir build`  into the terminal to create a build directory (libssh requires an out of source build.
    * Enter `cd build` into the terminal.
    * Enter `cmake  ../<EXTRACTED_DIR>` into the terminal  (where <EXTRACTED_DIR> is the directory of the extracted archive)
    to configure the installation & generate makefiles.
    * Enter `make`  into the terminal  to build libssh and then `sudo make install`to install libssh into your system path.

<a name="building"/>
Building:
-------------

* **NOTE:** On 64 bit systems, you might need to prepend the library path with `/usr/lib64` for NCV to
properly link with installed dependencies. In Qt Creator this can be found in **Projects**-> **Build Environment** -> **LD_LIBRARY_PATH** .

#### With Qt Creator:
* Enter `git clone https://github.com/alexander-jones/NCV.git` to download the newest version.
* Open NCV/ncv.pro in QtCreator and press F5 to build and run NCV.

#### With qmake:
* Enter `git clone https://github.com/alexander-jones/NCV.git` to download the newest version.
* Open a terminal and type `which qmake-qt4`. Copy the path that results (usually `/usr/bin/qmake-qt4`), navigate
to the directory containing NCV and enter `./<QMAKE_PATH> .` to build NCV.

<a name="running"/>
Running:
-------------

#### COMING SOON!

<a name="plugins"/>
Developing Plugins:
-------------------

#### COMING SOON!
