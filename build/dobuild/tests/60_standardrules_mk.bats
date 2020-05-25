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
  OUTDIR="$(mktemp --directory --tmpdir="$BATS_TMPDIR" standardrules_XXXXXXXXXX)"
  MAKE=("make" --no-print-directory -C "$PROJECTPATH" -f standardrules.mk -f "$BATS_TEST_DIRNAME/test_helper.mk")
  
  export DOBUILD_OUTDIR="$OUTDIR"
}

teardown() {
  rm -rf "$OUTDIR"
}

has_required_version() {
  local major=${1:-0}
  local minor=${2:-0}
  local bugfix=${3:-0}
  
  set -- $(sed -n -e 's/.* Make \([0-9]\+\)[.]\([0-9]\+\)\([.]\([0-9]\+\)\)\?/\1 \2 \4/p')
  
  if [ "$1" -gt "$major" ]; then
    return 0
  fi
  if [ "$1" -lt "$major" ]; then
    return -1
  fi

  if [ "$2" -gt "$minor" ]; then
    return 0
  fi
  if [ "$2" -lt "$minor" ]; then
    return -2
  fi

  if [ "${3:-0}" -ge "$bugfix" ]; then
    return 0
  fi
  
  return -3
}

make_has_required_version() {
  "${MAKE[@]}" --version | has_required_version "$@"
}

@test "standardrules_mk has required deps" {
  "${MAKE[@]}" --version
}

@test "standardrules_mk has required makefile deps" {
  "${MAKE[@]}" testhelper-print-MAKEFILE_DEPS | sort -u | parallel --keep-order command -V
}

@test "standardrules_mk PROJECTDIR should not contain whitespace" {
  run "${MAKE[@]}" PROJECTDIR='my project'
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -ne 0 ]
  [[ "${lines[@]}" =~ "Project directory PROJECTDIR='my project' should not contain whitespaces" ]]
}

@test "standardrules_mk DOBUILDDIR should not contain whitespace" {
  run "${MAKE[@]}" DOBUILDDIR='my scriptdir'
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -ne 0 ]
  [[ "${lines[@]}" =~ "Script directory DOBUILDDIR='my scriptdir' should not contain whitespaces" ]]
}

@test "standardrules_mk OUTDIRS should not contain PROJECTDIR" {
  run "${MAKE[@]}" PROJECTDIR="$PROJECTPATH" OUTDIR='.'
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -ne 0 ]
  [[ "${lines[@]}" =~ "assertion failed: Project location PROJECTDIR" ]]
  [[ "${lines[@]}" =~ "should not point to one of the output locations" ]]
}

@test "standardrules_mk all targets are tested" {
  run "${MAKE[@]}" print-targets 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = 'all' ]
  [ "${lines[1]}" = 'check' ]
  [ "${lines[2]}" = 'clean' ]
  [ "${lines[3]}" = 'dist' ]
  [ "${lines[4]}" = 'distclean' ]
  [ "${lines[5]}" = 'install' ]
  [ "${lines[6]}" = 'lint' ]
  [ "${lines[7]}" = 'memcheck' ]
  [ "${lines[8]}" = 'prepare' ]  
  [ "${lines[9]}" = 'print-targets' ]
  [ "${lines[10]}" = 'run' ]
  # do not forget to add a test :)
  [ "${#lines[@]}" -ge 11 ]
}

@test "standardrules_mk should run default target 'all' by default" {
  run "${MAKE[@]}" 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"all'." ]]
}

@test "standardrules_mk returns success when empty check target is triggered" {
  run "${MAKE[@]}" check 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"check'." ]]
}

@test "standardrules_mk returns success when empty clean target is triggered" {
  run "${MAKE[@]}" clean 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"clean'." ]]
}

@test "standardrules_mk returns success when empty distclean target is triggered" {
  run "${MAKE[@]}" distclean 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"distclean'." ]]
}

@test "standardrules_mk returns success when empty prepare target is triggered " {
  run "${MAKE[@]}" prepare 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"prepare'." ]]
}

@test "standardrules_mk returns success when empty lint target is triggered" {
  run "${MAKE[@]}" lint 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"lint'." ]]
}

@test "standardrules_mk returns success when empty memcheck target is triggered" {
  run "${MAKE[@]}" memcheck 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"memcheck'." ]]
}

@test "standardrules_mk returns success when empty run target is triggered" {
  run "${MAKE[@]}" run 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done for '[\`|\']"run'." ]]
}

@test "standardrules_mk returns error when makefile dependency are unsatisfied" {
  run "${MAKE[@]}" MAKEFILE_DEPS='not-found1 not-found2 cat' not-found1 2>&1
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -ne 0 ]
  [[ "${lines[@]}" =~ "Required commands not-found1 not-found2 not found; install appropriate packages" ]]
}

@test "standardrules_mk should print PROJECTNAME when target triggered" {
  EXPECTED="$(basename "$PROJECTPATH")"
  VALUE="$("${MAKE[@]}" debug-print-PROJECTNAME)"
  
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "standardrules_mk EXTERNPARALLEL should default to 1 when make called recursive with limited job slots" {
  EXPECTED="1"
  VALUE="$("${MAKE[@]}" -j10 testhelper-recursive-print-EXTERNPARALLEL)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "standardrules_mk INTERNPARALLEL should default to 2 when make called recursive with limited job slots" {
  EXPECTED="2"
  VALUE="$("${MAKE[@]}" -j10 testhelper-recursive-print-INTERNPARALLEL)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "standardrules_mk EXTERNPARALLEL * INTERNPARALLEL should not exceed provided job slots" {
  PARALLELMFLAGS='-j10'
  EXPECTED='10'
  EXTERNPARALLEL="$("${MAKE[@]}" "$PARALLELMFLAGS" testhelper-print-EXTERNPARALLEL)"
  INTERNPARALLEL="$("${MAKE[@]}" "$PARALLELMFLAGS" testhelper-print-INTERNPARALLEL)"
  VALUE="$(($EXTERNPARALLEL * $INTERNPARALLEL))"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "standardrules_mk EXTERNPARALLEL should not exceed active targets" {
  EXPECTED="$("${MAKE[@]}" testhelper-print-JOBS)"
  VALUE="$("${MAKE[@]}" testhelper-print-EXTERNPARALLEL)"
  echo "$VALUE <= $EXPECTED"
  [ "$VALUE" -le "$EXPECTED" ]
}

@test "standardrules_mk MAKEFLAGS should contain output-sync option when job slots greater than 1" {
  if ! make_has_required_version '4'; then
    skip "make version too old '$("${MAKE[@]}" --version)'"
  fi

  run "${MAKE[@]}" testhelper-print-MAKEFLAGS EXTERNPARALLEL="32"
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "-Otarget" ]]
}

@test "standardrules_mk MAKEFLAGS should not contain output-sync option when job slots equal to 1" {
  run "${MAKE[@]}" testhelper-print-MAKEFLAGS EXTERNPARALLEL="1"
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "-Otarget" ]]
}
