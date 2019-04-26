#!/bin/sh
#
# Note: lcov is needed to generate coverage report
#

mkdir -p .build/coverage
lcov --capture --directory . --output-file .build/coverage/lcov.info
genhtml .build/coverage/lcov.info --output-directory .build/coverage/report