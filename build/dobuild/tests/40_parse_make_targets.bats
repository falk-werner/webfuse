#!/usr/bin/env bats
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

load test_helper

setup() {
  BINPATH="$(readlink -f "$BATS_TEST_DIRNAME/../bin")"
  PATH="$BINPATH:$PATH"
}

@test "parse_make_targets has required dep awk" {
  awk 'BEGIN {print "Hello from awk"}'
}

@test "parse_make_targets has required dep grep" {
  echo 'hello' | grep -e 'hello'
}

@test "parse_make_targets has required dep sort" {
  sort --version
}

@test "parse_make_targets has required dep make" {
  make --version
}

print_simple_makefile() {
  cat <<EOF
.PHONY: default
default: all
.PHONY: all
all:
.PHONY: clean
clean:
EOF
}

print_error_makefile() {
  cat <<EOF
\$(error within Makefile)
EOF
}

@test "parse_make_targets should fail when used with invalid arguments" {
  run parse_make_targets.sh --unknown-make-option 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [ -n "$output" ]
}

@test "parse_make_targets should fail when makefile contains errors" {
  run parse_make_targets.sh -f -< <(print_error_makefile)
  
  echo "$output"
  [ "$status" -ne 0 ]
  [ -n "$output" ]
}

@test "parse_make_targets should parse simple makefile from stdin" {
  run parse_make_targets.sh -f -< <(print_simple_makefile)
  
  echo "$output"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = 'all' ]
  [ "${lines[1]}" = 'clean' ]
  [ "${lines[2]}" = 'default' ]
  [ "${#lines[@]}" -eq 3 ]
}

@test "parse_make_targets should parse a more advanced makefile" {
  run parse_make_targets.sh -C "$BATS_TEST_DIRNAME/fixtures/make-gtest-example"
  
  echo "$output"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = 'all' ]
  [ "${lines[1]}" = 'alltests' ]
  [ "${lines[2]}" = 'clean' ]
  [ "${#lines[@]}" -eq 3 ]
}


