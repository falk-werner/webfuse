# fuse-wsfs

fuse-wsfs combines libwebsockets and libfuse. It allows ot attach a remote filesystem via websockets.

**Status**: Proof of concept  
***Latest Build on travis***: [https://travis-ci.com/falk-werner/fuse-wsfs](https://travis-ci.com/falk-werner/fuse-wsfs)

# Build and run

    cd fuse-wsfs
    mkdir ./build
    cd ./build
    cmake .. -DWITH_TESTS=ON
    make
    CTEST_OUTPUT_ON_FAILUE=1 make test
    mkdir test
    ./wsfs -m test --document_root=`realpath ../src/app/www` --port=4711


# Dependencies

- [libfuse3](https://github.com/libfuse/libfuse/)
- [libwebsockets](https://libwebsockets.org/)
- [Jansson](https://jansson.readthedocs.io)
- [GoogleTest](https://github.com/google/googletest) *(optional)*


