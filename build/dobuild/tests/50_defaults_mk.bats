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
  MAKE=("make" --no-print-directory -C "$PROJECTPATH" -f defaults.mk -f "$BATS_TEST_DIRNAME/test_helper.mk")
  VERSIONFILE="$(mktemp --tmpdir="$BATS_TMPDIR" version_XXXXXXXXXX.txt)"
  
  export DOBUILD_PROJECTVERSIONFILE="$VERSIONFILE"
}

teardown() {
  rm -f "$VERSIONFILE"
}

@test "defaults_mk has required deps" {
  "${MAKE[@]}" --version
}

@test "defaults_mk has required makefile deps" {
  "${MAKE[@]}" testhelper-print-MAKEFILE_DEPS | sort -u | parallel --keep-order command -V
}

@test "defaults_mk can be included multiple times" {
  run "${MAKE[@]}" -f defaults.mk 2>&1
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ 'make: Nothing to be done' ]]
}

print_fixture_or() {
  cat <<EOF
VALUES += 1_\$(call or,)
VALUES += 2_\$(call or,a,b)
VALUES += 3_\$(call or,,,,b)
EOF
}

@test "defaults_mk test function or" {
  run eval 'print_fixture_or | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_" ]
  [ "${lines[1]}" = "2_a" ]
  [ "${lines[2]}" = "3_b" ]
  [ "${#lines[@]}" -eq 3 ]
}

print_fixture_lastword() {
  cat <<EOF
VALUES += 1_\$(call lastword,)
VALUES += 2_\$(call lastword,0 1)
VALUES += 3_\$(call lastword,1 2 3)
EOF
}

@test "defaults_mk test function lastword" {
  run eval 'print_fixture_lastword | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_" ]
  [ "${lines[1]}" = "2_1" ]
  [ "${lines[2]}" = "3_3" ]
  [ "${#lines[@]}" -eq 3 ]
}

print_fixture_eq_s() {
  cat <<EOF
VALUES += 1_\$(call eq_s,1,1)
VALUES += 2_\$(call eq_s,1,2)
VALUES += 3_\$(call eq_s,text,text)
VALUES += 4_\$(call eq_s,text,other)
EOF
}

@test "defaults_mk test function eq_s" {
  run eval 'print_fixture_eq_s | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_text" ]
  [ "${lines[3]}" = "4_" ]
  [ "${#lines[@]}" -eq 4 ]
}

print_fixture_tail() {
  cat <<EOF
VALUES += 1_\$(call tail,)
VALUES += 2_\$(call tail,1)
VALUES += 3_\$(call tail,1 42)
EOF
}

@test "defaults_mk test function tail" {
  run eval 'print_fixture_tail | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_42" ]
  [ "${#lines[@]}" -eq 3 ]
}

print_fixture_eq_each() {
  cat <<EOF
VALUES += 1_\$(call eq_each,1,1,eq_s)
VALUES += 2_\$(call eq_each,1,2,eq_s)
VALUES += 3_\$(call eq_each,text,text,eq_s)
VALUES += 4_\$(call eq_each,text,other,eq_s)
VALUES += 5_\$(call eq_each,a b,a b,eq_s)
VALUES += 6_\$(call eq_each,a b,a c,eq_s)
VALUES += 7_\$(call eq_each,a b,a,eq_s)
EOF
}

@test "defaults_mk test function eq_each" {
  run eval 'print_fixture_eq_each | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${lines[5]}" = "6_" ]
  [ "${lines[6]}" = "7_" ]
  [ "${#lines[@]}" -eq 7 ]
}

print_fixture_eq() {
  cat <<EOF
VALUES += 1_\$(call eq,1,1,eq_s)
VALUES += 2_\$(call eq,1,2)
VALUES += 3_\$(call eq,text,text)
VALUES += 4_\$(call eq,text,other)
VALUES += 5_\$(call eq,a b,a b)
VALUES += 6_\$(call eq,a b,a c)
VALUES += 7_\$(call eq,a b,a)
EOF
}

@test "defaults_mk test function eq" {
  run eval 'print_fixture_eq | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${lines[5]}" = "6_" ]
  [ "${lines[6]}" = "7_" ]
  [ "${#lines[@]}" -eq 7 ]
}

print_fixture_target_properties_matches() {
  cat <<EOF
VALUES += 1_\$(call target_properties_matches,1,1)
VALUES += 2_\$(call target_properties_matches,1,2)
VALUES += 3_\$(call target_properties_matches,text,text)
VALUES += 4_\$(call target_properties_matches,text,other)
VALUES += 5_\$(call target_properties_matches,%,text)
VALUES += 6_\$(call target_properties_matches,text,%)
VALUES += 7_\$(call target_properties_matches,a % c,a b c)
VALUES += 8_\$(call target_properties_matches,a d %,a b c)
VALUES += 9_\$(call target_properties_matches,a d,a d c)
EOF
}

@test "defaults_mk test function target_properties_matches" {
  run eval 'print_fixture_target_properties_matches | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${lines[5]}" = "6_" ]
  [ "${lines[6]}" = "7_1" ]
  [ "${lines[7]}" = "8_" ]
  [ "${lines[8]}" = "9_" ]
  [ "${#lines[@]}" -eq 9 ]
}

print_fixture_target_matches() {
  cat <<EOF
VALUES += 1_\$(call target_matches,x86_64+builder-template,x86_64 x86_64 unknown linux gnu builder-template latest release)
VALUES += 2_\$(call target_matches,i386+arm32v7-alpine@3.9-none-eabihf+builder-template,i386 arm32v7 alpine none eabihf builder-template 3.9 release)
VALUES += 3_\$(call target_matches,arm32v7-alpine@3.9-none-eabihf+builder-template,arm32v7 arm32v7 alpine % % % % %)
VALUES += 4_\$(call target_matches,arm32v7-alpine@3.9-none-eabihf+builder-template,% arm32v7 alpine % % % % %)
VALUES += 5_\$(call target_matches,i386+arm32v7-alpine@3.9-none-eabihf+builder-template,i386 arm32v7 ubuntu % % % % %)
VALUES += 6_\$(call target_matches,i386+arm32v7-alpine@3.9-none-eabihf+builder-template@debug,i386 arm32v7 alpine % % % % debug)
EOF
}

@test "defaults_mk test function target_matches" {
  run eval 'print_fixture_target_matches | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_1" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_1" ]
  [ "${lines[4]}" = "5_" ]
  [ "${lines[5]}" = "6_1" ]
  [ "${#lines[@]}" -eq 6 ]
}

print_fixture_target_filter() {
  cat <<EOF
VALUES += 1_\$(call target_filter,x86_64+x86_64-unknown@latest-linux-gnu+builder-template,x86_64+builder-template)
VALUES += 2_\$(call target_filter,i386+arm32v7-alpine@3.9-none-eabihf+builder-template,i386+arm32v7-alpine@3.9-none-eabihf+builder-template)
VALUES += 3_\$(call target_filter,arm32v7+arm32v7-alpine@%-%-%+%,arm32v7-alpine@3.9-none-eabihf+builder-template)
VALUES += 4_\$(call target_filter,%+arm32v7-alpine@%-%-%+%,arm32v7-alpine@3.9-none-eabihf+builder-template)
VALUES += 5_\$(call target_filter,i386+arm32v7-ubuntu@%-%-%+%,i386+arm32v7-alpine@3.9-none-eabihf+builder-template)
EOF
}

@test "defaults_mk test function target_filter" {
  run eval 'print_fixture_target_filter | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_x86_64+builder-template" ]
  [ "${lines[1]}" = "2_i386+arm32v7-alpine@3.9-none-eabihf+builder-template" ]
  [ "${lines[2]}" = "3_arm32v7-alpine@3.9-none-eabihf+builder-template" ]
  [ "${lines[3]}" = "4_arm32v7-alpine@3.9-none-eabihf+builder-template" ]
  [ "${lines[4]}" = "5_" ]
  [ "${#lines[@]}" -eq 5 ]
}


print_fixture_compare() {
  cat <<EOF
VALUES += 1_\$(call compare, 1, 2)
VALUES += 2_\$(call compare,-1,-2)
VALUES += 3_\$(call compare, 1, 1)
VALUES += 4_\$(call compare,-1, 2)
EOF
}

@test "defaults_mk test function compare" {
  run eval 'print_fixture_compare | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_-1" ]
  [ "${lines[1]}" = "2_1" ]
  [ "${lines[2]}" = "3_0" ]
  [ "${lines[3]}" = "4_-3" ]
  [ "${#lines[@]}" -eq 4 ]
}

print_fixture_lt() {
  cat <<EOF
VALUES += 1_\$(call lt, 1, 2)
VALUES += 2_\$(call lt,-1,-2)
VALUES += 3_\$(call lt, 1, 1)
VALUES += 4_\$(call lt,-1, 2)
VALUES += 5_\$(call lt, 4, 2)
EOF
}

@test "defaults_mk test function lt" {
  run eval 'print_fixture_lt | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_" ]
  [ "${lines[3]}" = "4_1" ]
  [ "${lines[4]}" = "5_" ]
  [ "${#lines[@]}" -eq 5 ]
}

print_fixture_gt() {
  cat <<EOF
VALUES += 1_\$(call gt, 1, 2)
VALUES += 2_\$(call gt,-1,-2)
VALUES += 3_\$(call gt, 1, 1)
VALUES += 4_\$(call gt,-1, 2)
VALUES += 5_\$(call gt, 4, 2)
EOF
}

@test "defaults_mk test function gt" {
  run eval 'print_fixture_gt | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_" ]
  [ "${lines[1]}" = "2_1" ]
  [ "${lines[2]}" = "3_" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${#lines[@]}" -eq 5 ]
}

print_fixture_ge() {
  cat <<EOF
VALUES += 1_\$(call ge, 1, 2)
VALUES += 2_\$(call ge,-1,-2)
VALUES += 3_\$(call ge, 1, 1)
VALUES += 4_\$(call ge,-1, 2)
VALUES += 5_\$(call ge, 4, 2)
EOF
}

@test "defaults_mk test function ge" {
  run eval 'print_fixture_ge | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_" ]
  [ "${lines[1]}" = "2_1" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${#lines[@]}" -eq 5 ]
}

print_fixture_ge_version() {
  cat <<EOF
VALUES += 1_\$(call ge_version, 3, 1)
VALUES += 2_\$(call ge_version, 1, 2)
VALUES += 3_\$(call ge_version, 1, 1)
VALUES += 4_\$(call ge_version, 1 1, 2 0)
VALUES += 5_\$(call ge_version, 1 2, 1 0)
VALUES += 6_\$(call ge_version, 1 1, 1 2)
VALUES += 7_\$(call ge_version, 1 1, 2 3)
VALUES += 8_\$(call ge_version, 1 1, 1)
VALUES += 9_\$(call ge_version, 1 1 1, 1)
VALUES += 10_\$(call ge_version, 1, 1 1 1)
VALUES += 11_\$(call ge_version, 1 2, 1 1 1)
VALUES += 12_\$(call ge_version, 1 2 1, 1 2 0)
VALUES += 13_\$(call ge_version, , 1)
VALUES += 14_\$(call ge_version, 2, )
VALUES += 15_\$(call ge_version, , )
EOF
}

@test "defaults_mk test function ge_version" {
  run eval 'print_fixture_ge_version | "${MAKE[@]}" -f - testhelper-print-VALUES'

  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_1" ]
  [ "${lines[1]}" = "2_" ]
  [ "${lines[2]}" = "3_1" ]
  [ "${lines[3]}" = "4_" ]
  [ "${lines[4]}" = "5_1" ]
  [ "${lines[5]}" = "6_" ]
  [ "${lines[6]}" = "7_" ]
  [ "${lines[7]}" = "8_1" ]
  [ "${lines[8]}" = "9_1" ]
  [ "${lines[9]}" = "10_" ]
  [ "${lines[10]}" = "11_1" ]
  [ "${lines[11]}" = "12_1" ]
  [ "${lines[12]}" = "13_" ]
  [ "${lines[13]}" = "14_1" ]
  [ "${lines[14]}" = "15_1" ]
  [ "${#lines[@]}" -eq 15 ]
}

@test "defaults_mk PROJECTNAME equals directory of first loaded makefile by default and can't be overridden accidentally" {
  EXPECTED="$(basename "$PROJECTPATH")"
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTNAME)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
  
  export PROJECTNAME='myproject'
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTNAME)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk PROJECTNAME can be overridden by env var DOBUILD_PROJECTNAME" {
  export DOBUILD_PROJECTNAME='myproject'
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTNAME)"
  echo "$VALUE"
  [ "$VALUE" = "myproject" ]
}

@test "defaults_mk PROJECTDIR equals directory of first loaded makefile by default and can't be overridden accidentally" {
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTDIR)"
  echo "$VALUE != ."
  [ "$VALUE" = '.' ]
  
  export PROJECTDIR='myproject'
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTDIR)"
  echo "$VALUE != ."
  [ "$VALUE" = '.' ]
}

@test "defaults_mk PROJECTDIR can be overridden by env var DOBUILD_PROJECTDIR" {
  export DOBUILD_PROJECTDIR='myproject'
  VALUE="$("${MAKE[@]}" testhelper-print-PROJECTDIR)"
  echo "$VALUE"
  [ "$VALUE" = "myproject" ]
}

@test "defaults_mk USERID equals UID by default and can't be overridden accidentally" {
  EXPECTED="$(id -u)"
  VALUE="$("${MAKE[@]}" testhelper-print-USERID)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
  
  export USERID='an_user'
  VALUE="$("${MAKE[@]}" testhelper-print-USERID)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk USERID can be overridden by env var DOBUILD_USERID" {
  EXPECTED="1234"
  export DOBUILD_USERID="$EXPECTED"
  VALUE="$("${MAKE[@]}" testhelper-print-USERID)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk VERSION is empty when no version file exists" {
  VALUE="$("${MAKE[@]}" testhelper-print-VERSION VERSIONFILE='')"
  echo "$VALUE"
  [ -z "$VALUE" ]
}

@test "defaults_mk VERSION equals version in provided file" {
  EXPECTED='99.99.99'
  echo "$EXPECTED" > "$VERSIONFILE"
  VALUE="$("${MAKE[@]}" testhelper-print-VERSION)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk HOST_CONTAINER can be overridden by env var DOBUILD_HOSTCONTAINER" {
  EXPECTED="1234"
  export DOBUILD_HOSTCONTAINER="$EXPECTED"
  VALUE="$("${MAKE[@]}" testhelper-print-HOST_CONTAINER)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk SOURCE_DATE_EPOCH is number" {
  VALUE="$("${MAKE[@]}" testhelper-print-SOURCE_DATE_EPOCH)"
  [ "$VALUE" -ge 0 ]
}

@test "defaults_mk SOURCE_DATE_EPOCH can be overridden" {
  EXPECTED="1234"
  VALUE="$("${MAKE[@]}" testhelper-print-SOURCE_DATE_EPOCH SOURCE_DATE_EPOCH="$EXPECTED")"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" = "$EXPECTED" ]
}

@test "defaults_mk BUILDTIME is not empty" {
  VALUE="$("${MAKE[@]}" testhelper-print-BUILDTIME)"
  [ -n "$VALUE" ]
}

@test "defaults_mk VERBOSE is off by default and can be overridden by env var" {
  VALUE="$("${MAKE[@]}" testhelper-default-silent)"
  echo "$VALUE"
  [ -z "$VALUE" ]
  
  export VERBOSE=1
  VALUE="$("${MAKE[@]}" testhelper-default-silent)"
  echo "$VALUE"
  [ "$VALUE" = 'true' ]
}

@test "defaults_mk JOBSLOTS defaults to system available processors" {
  EXPECTED="$(nproc)"
  VALUE="$("${MAKE[@]}" testhelper-print-JOBSLOTS)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "defaults_mk JOBSLOTS defaults to system available processors when make called recursive without -j" {
  EXPECTED="$(nproc)"
  VALUE="$("${MAKE[@]}" testhelper-recursive-print-JOBSLOTS)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "defaults_mk JOBSLOTS defaults to system available processors when make called recursive with unlimited job slots" {
  EXPECTED="$(nproc)"
  VALUE="$("${MAKE[@]}" -j testhelper-recursive-print-JOBSLOTS)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "defaults_mk JOBSLOTS defaults to 2 when make called recursive with limited job slots" {
  EXPECTED="2"
  VALUE="$("${MAKE[@]}" -j10 testhelper-recursive-print-JOBSLOTS)"
  echo "$VALUE != $EXPECTED"
  [ "$VALUE" -eq "$EXPECTED" ]
}

@test "defaults_mk JOBSLOTS should be equal to last make command line argument" {
  VALUE="$("${MAKE[@]}" -j10 -j   42 testhelper-print-JOBSLOTS)"
  echo "$VALUE != 42"
  [ "$VALUE" -eq "42" ]

  VALUE="$("${MAKE[@]}" -j14 testhelper-print-JOBSLOTS)"
  echo "$VALUE != 14"
  [ "$VALUE" -eq "14" ]
  
  VALUE="$("${MAKE[@]}" -j11 testhelper-print-JOBSLOTS)"
  echo "$VALUE != 11"
  [ "$VALUE" -eq "11" ]

  VALUE="$("${MAKE[@]}" --jobs 11 --jobs 	10 testhelper-print-JOBSLOTS)"
  echo "$VALUE != 10"
  [ "$VALUE" -eq "10" ]

  VALUE="$("${MAKE[@]}" -j10 --jobs=88 testhelper-print-JOBSLOTS)"
  echo "$VALUE != 88"
  [ "$VALUE" -eq "88" ]
}

@test "defaults_mk MAKE_VERSION should be greater or equal to 3.81 warning" {
  run "${MAKE[@]}" make_version=4.1 testhelper-print-EXPECTATIONS 2>&1
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ ! "${lines[@]}" =~ "Using old make version" ]]

  run "${MAKE[@]}" make_version=3.80 testhelper-print-EXPECTATIONS 2>&1
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [[ "${lines[@]}" =~ "Using old make version" ]]
}

