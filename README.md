# fuse-wsfs

fuse-wsfs combines libwebsockets and libfuse. It allows ot attach a remote filesystem via websockets.

**Status**: Proof of concept  
***Latest Build on travis***: [https://travis-ci.com/falk-werner/fuse-wsfs](https://travis-ci.com/falk-werner/fuse-wsfs)

# Build and run

To install dependencies, see below.

    cd fuse-wsfs
    mkdir ./build
    cd ./build
    cmake -DWITH_EXAMPLE=ON -DWITH_TESTS=ON ..
    make
    CTEST_OUTPUT_ON_FAILUE=1 make test
    mkdir test
    ./wsfs -m test --document_root=`realpath ../exmaple/www` --port=4711


# Dependencies

- [libfuse3](https://github.com/libfuse/libfuse/)
- [libwebsockets](https://libwebsockets.org/)
- [Jansson](https://jansson.readthedocs.io)
- [GoogleTest](https://github.com/google/googletest) *(optional)*

## Installation

### libfuse

    wget -O fuse-3.1.1.tar.gz https://github.com/libfuse/libfuse/archive/fuse-3.1.1.tar.gz
    tar -xf fuse-3.1.1.tar.gz
    cd libfuse-fuse-3.1.1
    ./makeconf.sh
    ./configure
    make
    sudo make install

### libwebsockets

    wget -O libwebsockets-3.1.0.tar.gz https://github.com/warmcat/libwebsockets/archive/v3.1.0.tar.gz
    tar -xf libwebsockets-3.1.0.tar.gz
    cd libwebsockets-3.1.0
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

### Jansson

On many systems, libjansson can installed via apt:
    
    sudo apt install libjansson-dev

Otherwise, it can be installed from source:
    
    wget -O libjansson-2.12.tar.gz https://github.com/akheron/jansson/archive/v2.12.tar.gz
    tar -xf libjansson-2.12.tar.gz
    cd jansson-2.12
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

### GoogleTest

Installation of GoogleTest is optional fuse-wsfs library, but required to compile tests.

    wget -O gtest-1.8.1.tar.gz https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    tar -xf gtest-1.8.1.tar.gz
    cd googletest-release-1.8.1
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install


