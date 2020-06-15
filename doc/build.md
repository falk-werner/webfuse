# Build Instructions

To build webfuse from source, meson is needed.
Please refer to [meson quick guide](https://mesonbuild.com/Quick-guide.html) for setup instructions.

    meson .build
    cd .build
    ninja build

## Build options

Build options can be specified during meson setup or later via meson configure.

    meson -D<option>=<value> .build

The following options are available:

-   **without_tests**: _(boolean)_ diable tests  
    `meson -Dwithout_tests=true .build`

-   **without_adapter**: _(boolean)_ omit adapter library  
    `meson -Dwithout_adapter=true .build`

-   **without_provider**: _(boolean)_ omit provider library
    `meson -Dwithout_provider=true .build`

_Note that unit tests are only available, when both libraries are built._

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

    wget -O fuse-3.9.2.tar.gz https://github.com/libfuse/libfuse/archive/fuse-3.9.2.tar.gz
    tar -xf fuse-3.9.2.tar.gz
    cd libfuse-fuse-3.9.2
    mkdir .build
    cd .build
    meson ..
    ninja
    sudo ninja install

#### libwebsockets

    wget -O libwebsockets-4.0.10.tar.gz https://github.com/warmcat/libwebsockets/archive/v4.0.10.tar.gz
    tar -xf libwebsockets-4.0.10.tar.gz
    cd libwebsockets-4.0.10
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
