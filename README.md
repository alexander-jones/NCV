Building NCV:
=============

Prerequisites:
--------------

1. In order for the visualization plugin to render properly your host system must have a GPU with drivers capable of
creating a OpenGL 3.3 context.
2. You must be running a Windows 7, Linux, or Mac OSX operating system.

On Linux - 
----------

### Install Dependencies:

1. Multiple libraries needed by NCV require CMake to generate makefiles, so if your system does not contain CMake
enter `sudo apt-get install cmake` into a terminal

##### First, install the Qt 4.8.4 Dependencies:

1. Download the Qt 4.8.4 archive from [here](http://qt-project.org/downloads). 
2. Extract the  Qt 4.8.4  archive and navigate into the extracted directory with a terminal.
3. Within the terminal type `./configure` to configure the installation & generate makefiles.
4. Once this is completed, type `make` into the terminal to build Qt 4.8.4.
5. Type `sudo make install` into the terminal to install Qt 4.8.4 into your system path.
6. Type `sudo apt-get install libqxt-dev` to install the libQxt extenstion library.

#####  Second, install the libssh 0.5.0 library:

0. This library depends on libXmu and libXu to compile. To install these dependencies
enter `sudo apt-get install libxmu-dev libxu-dev` into a terminal.
1. Download the libssh archive from [here](http://www.libssh.org/files/0.5/). 
2. Extract the  libssh  archive and open a terminal.
3. Libssh requires an out of source build, so in order to build the library you need to create
a separate directory that houses the built files. Navigate to the directory containing the extracted archive 
folder enter `mkdir build`.
4. Enter `cd build`.
5. Assuming the folder you extracted libssh to was named `libssh-0.5.0`, type `cmake  ../libssh-0.5.0` to 
configure the installation & generate makefiles.
6. Once this is completed, type `make` to build libssh.
7. Type `sudo make install` into the terminal to install libssh into your system path.

##### Lastly, install the GLEW 1.9.0 (OpenGL Extension Wrangler) library :

1. Download the GLEW archive from [here](http://www.libssh.org/files/0.5/). 
2. Extract the GLEW  archive and navigate into the extracted directory with a terminal.
3. Enter `cmake .` to  configure the installation & generate makefiles.
6. Once this is completed, type `make` to build GLEW.
7. Type `sudo make install.all` into the terminal to install GLEW into your system path.

##### The best option for developing NCV within Linux is Qt Creator. To install Qt Creator:
1. Download the Qt Creator application from [here](http://qt-project.org/downloads). 
2. Navigate into the directory the application was download into with a terminal.
3. Type `chmod +x` to make the application executable.
4. Run the executable by typing `./<EXECUTABLE_NAME>.run `

### Compiling NCV :
0. For good measure, enter `ldconfig` into a terminal to reconfigure your systems dynamic linker run-time bindings.
1. Enter `git clone https://github.com/alexander-jones/NCV.git' to download the newest version.
2. Open NCV/ncv.pro in QtCreator.
3. Press F5 to build and run NCV.
4. **NOTE:** On 64 bit systems, you might need to prepend the build path with `/usr/lib64` for NCV to
properly link with installed dependencies. In Qt Creator this can be found in *Projects-> Build Environment -> LD_LIBRARY_PATH* .

