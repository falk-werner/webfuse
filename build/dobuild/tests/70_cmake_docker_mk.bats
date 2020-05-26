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
  FIXTUREPATH="$(readlink -f "$BATS_TEST_DIRNAME/fixtures/cmake-gtest-example")"
  BUILDDIR="$(mktemp --directory --tmpdir="$BATS_TMPDIR" cmake_XXXXXXXXXX)"
  MAKE=(timeout --preserve-status --signal=SIGTERM 300 "make" --no-print-directory -C "$FIXTUREPATH" -f Makefile -f "$BATS_TEST_DIRNAME/test_helper.mk")
  
  export DOBUILD_BUILDDIR="$BUILDDIR"
}

teardown() {
  rm -rf "$BUILDDIR"
}

@test "cmake_docker_mk has required deps" {
  "${MAKE[@]}" --version
}

@test "cmake_docker_mk has required makefile deps" {
  "${MAKE[@]}" testhelper-print-MAKEFILE_DEPS | sort -u | parallel --keep-order command -V
}

@test "cmake_docker_mk should run default target 'all' with make generator by default" {
  run "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Configuring done" ]]
  [[ "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  run "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ ! "${lines[@]}" =~ "Linking CXX executable alltests" ]]
}

@test "cmake_docker_mk build should be reproducible using make generator" {
  run "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Configuring done" ]]
  [[ "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  mv "$BUILDDIR/.build/"*/alltests "$BUILDDIR"
  rm -rf "$BUILDDIR/.build"
  
  run "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]

  diff <(xxd "$BUILDDIR/.build/"*/alltests) <(xxd "$BUILDDIR/alltests")
}

@test "cmake_docker_mk should run default target 'all' with ninja generator" {
  run "${MAKE[@]}" CMAKE_GENERATOR=ninja
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Configuring done" ]]
  [[ "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  run "${MAKE[@]}" CMAKE_GENERATOR=ninja
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "ninja: no work to do" ]]
}

@test "cmake_docker_mk build should be reproducible using ninja generator" {
  run "${MAKE[@]}" CMAKE_GENERATOR=ninja
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Configuring done" ]]
  [[ "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  mv "$BUILDDIR/.build/"*/alltests "$BUILDDIR"
  rm -rf "$BUILDDIR/.build"
  
  run "${MAKE[@]}" CMAKE_GENERATOR=ninja
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  diff <(xxd "$BUILDDIR/.build/"*/alltests) <(xxd "$BUILDDIR/alltests")
}

@test "cmake_docker_mk should run tests when check target is triggered after a successful built" {
  run "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  run "${MAKE[@]}" check
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "100% tests passed, 0 tests failed out of 1" ]]

  run "${MAKE[@]}" check
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "100% tests passed, 0 tests failed out of 1" ]]
}

@test "cmake_docker_mk should run tests with memcheck when memcheck target is triggered after a successful built" {
  run debug_trace "${MAKE[@]}"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Linking CXX executable alltests" ]]
  
  "${MAKE[@]}" debug-print-MEMCHECKFILTER
  run debug_trace "${MAKE[@]}" memcheck
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Memory check" ]]
  [[ "${lines[@]}" =~ "100% tests passed, 0 tests failed out of 1" ]]

  "${MAKE[@]}" debug-print-MEMCHECKFILTER
  run debug_trace "${MAKE[@]}" memcheck
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Configuring done" ]]
  [[ ! "${lines[@]}" =~ "Generating done" ]]
  [[ "${lines[@]}" =~ "Memory check" ]]
  [[ "${lines[@]}" =~ "100% tests passed, 0 tests failed out of 1" ]]
}

