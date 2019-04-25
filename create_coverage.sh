#!/bin/sh
#
# Note: lcov is needed to generate coverage report
#

mkdir -p coverage
lcov --capture --directory . --output-file coverage/lcov.info
genhtml coverage/lcov.info --output-directory coverage/report