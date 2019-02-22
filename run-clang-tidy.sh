#!/bin/bash
#
# Helper to run clang-tidy to perform some additional checks.

FILES=`find lib -name '*.c'`
INCLUDES="-Iinclude -Ilib `pkg-config fuse3 --cflags` `pkg-config libwebsockets --cflags`"

clang-tidy -header-filter='.*' ${FILES} -- ${INCLUDES}
