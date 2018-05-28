Google Protobuf's on Ubuntu 16.04 LTS.

===
C++ Language:

Google protobuf's need to be installed as is written in here:

  https://github.com/google/protobuf/blob/master/src/README.md

However, to work with protobuf-c, it needs to be on Tag v3.4.1.  Example:

$ git clone https://github.com/google/protobuf.git
$ cd protobuf
$ git checkout v3.4.1
$ git submodule update --init --recursive
$ ./autogen.sh

After that, it builds fine with this:

$ ./configure
$ make
$ sudo make install
$ sudo ldconfig # refresh shared library cache.

Note that this includes the Python Generator.

===
C Language:

For the C lanaguage API, as noted in here, but make sure LD_LIBRARY_PATH
is set:

  export LD_LIBRARY_PATH=/usr/local/lib
  https://github.com/protobuf-c/protobuf-c

===
ARM Cross Compiling:

Although it can be done, it is suggested that it be done within Yocto
and/or BuildRoot.

In Yocto, Meta-Layers exist for both C and C++:

   https://layers.openembedded.org/layerindex/recipe/50693/
   https://layers.openembedded.org/layerindex/recipe/5442/

In BuildRoot, it's part of the configuration.

