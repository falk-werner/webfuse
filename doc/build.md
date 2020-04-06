# Build Instructions

To install dependencies, see below.

    cd webfuse
    mkdir .build
    cd .build
    cmake ..
    make

## Build options

By default, unit tests are enabled. You can disable them using the following cmake options:

-   **WITHOUT_TESTS**: disable tests  
    `cmake -DWITHOUT_TESTS=ON ..`

Since webfuse consists of two libraries, it is possible to disable one of them
in order to reduce build dependencies.  
*Note that unit tests are only available, when both libraries are built.*

-   **WITHOUT_ADAPTER**: omit adapter library  
    `cmake -DWITHOUT_ADAPTER=ON`

-   **WIHTOU_PROVIDER**: omit provider library
    `cmake -DWITHOUT_PROVIDER=ON`

## Create API documentation

To create API documentation, you must install doxygen and dot first.
After that, run doxygen in the project root directory.

    doxygen

After that, you will find the API documentation in the doc/api subdirectory.

## Dependencies

-   [libfuse3](https://github.com/libfuse/libfuse/)
-   [libwebsockets](https://libwebsockets.org/)
-   [Jansson](https://jansson.readthedocs.io)
-   [GoogleTest](https://github.com/google/googletest) *(optional)*

### Installation from source

#### libfuse

To install libfuse, meson is needed. Please refer to [meson quick guide](https://mesonbuild.com/Quick-guide.html) for setup instructions.

    wget -O fuse-3.9.1.tar.gz https://github.com/libfuse/libfuse/archive/fuse-3.9.1.tar.gz
    tar -xf fuse-3.9.1.tar.gz
    cd libfuse-fuse-3.9.1
    mkdir .build
    cd .build
    meson ..
    ninja
    sudo ninja install

#### libwebsockets

    wget -O libwebsockets-4.0.1.tar.gz https://github.com/warmcat/libwebsockets/archive/v4.0.1.tar.gz
    tar -xf libwebsockets-4.0.1.tar.gz
    cd libwebsockets-4.0.1
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

#### Jansson

    wget -O libjansson-2.12.tar.gz https://github.com/akheron/jansson/archive/v2.12.tar.gz
    tar -xf libjansson-2.12.tar.gz
    cd jansson-2.12
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

#### GoogleTest

Installation of GoogleTest is optional webfuse library, but required to compile tests.

    wget -O gtest-1.10.0.tar.gz https://github.com/google/googletest/archive/release-1.10.0.tar.gz
    tar -xf gtest-1.10.0.tar.gz
    cd googletest-release-1.10.0
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install
