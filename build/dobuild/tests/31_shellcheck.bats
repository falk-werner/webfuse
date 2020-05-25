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
  PROJECTPATH="$(readlink -f "$BATS_TEST_DIRNAME/..")"
  BINPATH="$PROJECTPATH/bin"
  PATH="$BINPATH:$PATH"
  
  SHELLCHECK=( shellcheck --color=never --format=gcc -x )
}

@test "shellcheck has required deps" {
  "${SHELLCHECK[@]}" --version
}

@test "shellcheck run_tests" {
  "${SHELLCHECK[@]}" "$PROJECTPATH/run_tests"
}

@test "shellcheck helper scripts" {
  find "$BINPATH" -type f -print0 | parallel --keep-order -0 "${SHELLCHECK[@]}" {}
}

@test "shellcheck test helper scripts" {
  find "$BATS_TEST_DIRNAME" -type f -name '*.bash' -print0 | parallel --keep-order -0 "${SHELLCHECK[@]}" {}
}

