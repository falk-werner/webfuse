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
  HOST_ARCH="$(uname -m)"
}

@test "parse_target_properties.sh has required deps" {
  sed --version
  paste --version
  uname --version
  cat --version
}

@test "parse_target_properties.sh fails when called without target name" {
  run parse_target_properties.sh
  
  echo "$output"
  [ "$status" -ne 0 ]
  [ -n "$output" ]  
}

@test "parse_target_properties.sh should parse shortest target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh app-id )
  
  [ "${PROPERTIES[0]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[1]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'app-id' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  [ "${PROPERTIES[7]}" = 'release' ]
}

@test "parse_target_properties.sh should parse host-march and id target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh x86_64+builder-template )
  
  [ "${PROPERTIES[0]}" = 'x86_64' ]
  [ "${PROPERTIES[1]}" = 'x86_64' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  
  mapfile -t PROPERTIES < <( parse_target_properties.sh x86_64 builder-template )
  
  [ "${PROPERTIES[0]}" = 'x86_64' ]
  [ "${PROPERTIES[1]}" = 'x86_64' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
}

@test "parse_target_properties.sh should parse longest target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh i386+arm32v7-alpine@3.9-none-eabihf+builder-template )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'none' ]
  [ "${PROPERTIES[4]}" = 'eabihf' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = '3.9' ]
  
  mapfile -t PROPERTIES < <( parse_target_properties.sh i386 arm32v7-alpine@3.9-none-eabihf builder-template )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'none' ]
  [ "${PROPERTIES[4]}" = 'eabihf' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = '3.9' ]
}

@test "parse_target_properties.sh should parse distribution version containing unescaped special character [@]" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh i386+arm32v7-alpine@3.9@345+builder-template~@y )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template@y' ]
  [ "${PROPERTIES[6]}" = '3.9@345' ]

  mapfile -t PROPERTIES < <( parse_target_properties.sh i386 arm32v7-alpine@3.9@345 builder-template~@y )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template@y' ]
  [ "${PROPERTIES[6]}" = '3.9@345' ]
}

@test "parse_target_properties.sh should parse target abi containing unescaped special character [-]" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh arm32v7-alpine@3.9-linux-gnu-x+builder-template )
  
  [ "${PROPERTIES[0]}" = 'arm32v7' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu-x' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = '3.9' ]

  mapfile -t PROPERTIES < <( parse_target_properties.sh arm32v7-alpine@3.9-linux-gnu-x builder-template )
  
  [ "${PROPERTIES[0]}" = 'arm32v7' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu-x' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = '3.9' ]
}

@test "parse_target_properties.sh should parse application id containing unescaped special characters [-]" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh i386+builder-template@y-z )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'i386' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  [ "${PROPERTIES[7]}" = 'y-z' ]
  
  mapfile -t PROPERTIES < <( parse_target_properties.sh i386 builder-template@y-z )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'i386' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  [ "${PROPERTIES[7]}" = 'y-z' ]
}

@test "parse_target_properties.sh should parse application id containing unescaped special character [+], when host arch is given" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh + )
  
  [ "${PROPERTIES[0]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[1]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = '+' ]
  [ "${PROPERTIES[6]}" = 'latest' ]

  mapfile -t PROPERTIES < <( parse_target_properties.sh i386+arm32v7+builder-template@x+y-z )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  [ "${PROPERTIES[7]}" = 'x+y-z' ]

  mapfile -t PROPERTIES < <( parse_target_properties.sh i386 arm32v7 builder-template@x y-z )
  
  [ "${PROPERTIES[0]}" = 'i386' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  [ "${PROPERTIES[7]}" = 'x+y-z' ]
}

@test "parse_target_properties.sh should parse escaped chars [+|\\s] in target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh 'x86~+arm32~ ~~-x-alpine~+app' )
  
  [ "${PROPERTIES[0]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[1]}" = "$HOST_ARCH" ]
  [ "${PROPERTIES[2]}" = 'unknown' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu' ]
  [ "${PROPERTIES[5]}" = 'x86+arm32 ~-x-alpine+app' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
}

@test "parse_target_properties.sh should parse escaped char [@] in target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh arm32v7-alpine~@3.9-linux-gnu-x+builder-template )

  [ "${PROPERTIES[0]}" = 'arm32v7' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'alpine@3.9' ]
  [ "${PROPERTIES[3]}" = 'linux' ]
  [ "${PROPERTIES[4]}" = 'gnu-x' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
}

@test "parse_target_properties.sh should parse escaped char [-] in target name" {
  mapfile -t PROPERTIES < <( parse_target_properties.sh arm32v7-linaro~-gcc-none-eabihf+builder-template )

  [ "${PROPERTIES[0]}" = 'arm32v7' ]
  [ "${PROPERTIES[1]}" = 'arm32v7' ]
  [ "${PROPERTIES[2]}" = 'linaro-gcc' ]
  [ "${PROPERTIES[3]}" = 'none' ]
  [ "${PROPERTIES[4]}" = 'eabihf' ]
  [ "${PROPERTIES[5]}" = 'builder-template' ]
  [ "${PROPERTIES[6]}" = 'latest' ]
  
}

