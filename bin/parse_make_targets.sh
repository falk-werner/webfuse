#!/bin/sh
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

set -e

# Derived based on https://stackoverflow.com/questions/4219255/how-do-you-get-the-list-of-targets-in-a-makefile
#
# Note: This relies on -p printing the database nonetheless, which is the case as of GNU make 3.82.
# Sadly, GNU make offers no direct option to just print the database.
#
# Invokes make in order to print the database derived from a makefile:
#     -p     prints the database
#     -Rr    suppresses inclusion of built-in rules and variables
#     -q     only tests the up-to-date-status of a target (without remaking anything), but that by itself doesn't 
#            prevent execution of recipe commands in all cases
#     "$@"   list of separate commandline parameters (additional make parameters)
#     :      is a deliberately invalid target that is meant to ensure that no commands are executed;
#   2>&1 1>&3
#    redirect stderr to stdout and stdout to fd 3 to allow filtering unwanted error message
#   grep -v '.*\s\+No rule to make target\s\+.*:.*' 1>&2 
#     suppresses the resulting error message - "make: *** No rule to make target ':'.  Stop." and redirect
#     other output to stderr
#   3>&1     redirect fd 3 back to stdout
print_make_db() {
  { "$MAKE" -pRrq "$@" : 2>&1 1>&3 | grep -v '.*\s\+No rule to make target\s\+.*:.*' 1>&2; } 3>&1
}

export LANG=C
export LC_ALL=C

MAKE="${MAKE:-make}"

print_make_db "$@" \
  | awk -v RS= -F: '/^# File/,/^# Finished Make database/ {if ($1 !~ "^[#.]") {print $1}}' \
  | sort \
  | grep -v -e '^[^[:alnum:]]' -e '[%]$'

# Pipe-chain explanation:
# awk -v RS= -F: '/^# File/,/^# Finished Make database/ {if ($1 !~ "^[#.]") {print $1}}'
#     v RS=  this is an awk idiom that breaks the input into blocks of contiguous non-empty lines.
#     -F:    input field speparator
#     /^# File/,/^# Finished Make database/
#       matches the range of lines in the output that contains all targets (true as of GNU make 3.82) - by limiting 
#       parsing to this range, there is no need to deal with false positives from other output sections.
#     if ($$1 !~ "^[#.]")
#       selectively ignores blocks:
#         # ... ignores non-targets, whose blocks start with # Not a target:
#         . ... ignores special targets
#       all other blocks should each start with a line containing only the name of an explicitly defined target 
#       followed by :
# sort
#   sorts the resulting list of targets, which is the best option, since not sorting doesn't produce a helpful 
#   ordering in that the order in which the targets appear in the makefile is not preserved.
# grep -v -e '^[^[:alnum:]]' -e '[%]$$' removes unwanted targets from the output:
#   -v       revert machtes
#   -e '^[^[:alnum:]]' 
#     ... hidden targets, which - by convention - are targets that start neither with a letter nor a digit.
#   -e '[%]$$'
#     ... targets ending with % (pattern targets)
