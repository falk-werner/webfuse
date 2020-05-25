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

@test "is_running_in_bg has required dep 'ps'" {
  ps --version
  echo "ps incompatible"
  ps -o stat= -p "$$"
}

@test "is_running_in_bg has required dep 'grep'" {
  grep --help
  echo "grep incompatible"
  echo 'Ss' | grep -vq -e '+$'
}

@test "is_running_in_bg returns error when ps fails" {
  ps() {
    return 1
  }
  
  run source is_running_in_bg.sh 1234
  echo "$output"
  [ "$status" -ne 0 ]
}

@test "is_running_in_bg returns success when running in bg" {
  ps() {
    echo 'Ss'
  }
  
  run source is_running_in_bg.sh 1234
  echo "$output"
  [ "$status" -eq 0 ]
  
  is_running_in_bg.sh &
}

@test "is_running_in_bg returns error when running in fg" {
  ps() {
    echo 'Ss+'
  }
  
  run source is_running_in_bg.sh 1234
  echo "$output"
  [ "$status" -ne 0 ]
}
