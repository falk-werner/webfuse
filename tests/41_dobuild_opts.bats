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
  TEMP="$(mktemp --directory --tmpdir="$BATS_TMPDIR" dobuild_XXXXXXXXXX)"
}

teardown() {
  rm -rf "$TEMP"
}

@test "dobuild has required dep find" {
  find --version
}

@test "dobuild has required dep dirname" {
  dirname --version
}

@test "dobuild has required dep mkdir" {
  mkdir --version
}

@test "dobuild has required dep grep" {
  grep --version
}

@test "dobuild has required dep sed" {
  sed --version
}

@test "dobuild should fail when -f, --file or --makefile argument is empty" {
  export MAKE='echo'
  
  run dobuild -f 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"-f" requires a non-empty option argument' ]]
  
  run dobuild --file 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--file" requires a non-empty option argument' ]]

  run dobuild --file= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--file=" requires a non-empty option argument' ]]
  
  run dobuild --makefile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--makefile" requires a non-empty option argument' ]]
  
  run dobuild --makefile= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--makefile=" requires a non-empty option argument' ]]
}

@test "dobuild should pass -f, --file or --makefile argument to make" {
  export MAKE='echo'
  
  run dobuild -f Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ '-f Makefile' ]]
  
  run dobuild --file Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ '--file Makefile' ]]
  
  run dobuild --file=Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ '--file=Makefile' ]]
}

@test "dobuild should pass all -f, --file or --makefile arguments to make and use first to determine project kind" {
  export MAKE='echo'
  
  run dobuild -f Makefile -f other.mk
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ '-f Makefile -f other.mk' ]]
}

@test "dobuild should fail when -C, --directory argument is empty" {
  export MAKE='echo'
  
  run dobuild -C 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"-C" requires a non-empty option argument' ]]
  
  run dobuild --directory 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--directory" requires a non-empty option argument' ]]

  run dobuild --directory= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--directory=" requires a non-empty option argument' ]]
}

@test "dobuild should not pass -C, --directory arguments to make" {
  export MAKE='echo'
  
  run dobuild -C "$TEMP" -f Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ ! "$output" =~ "-C $TEMP" ]]
  
  run dobuild --directory "$TEMP" -f Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ ! "$output" =~ "--directory $TEMP" ]]
  
  run dobuild --directory="$TEMP" -f Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ ! "$output" =~ "--directory=$TEMP" ]]
}

@test "dobuild should fail when directory denoted by -C, --directory does not exist" {
  export MAKE='echo'
  
  run dobuild -C non-existent -f Makefile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ "can't cd to non-existent" ]]
  
  run dobuild --directory non-existent -f Makefile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ "can't cd to non-existent" ]]
  
  run dobuild --directory=non-existent -f Makefile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ "can't cd to non-existent" ]]
}

@test "dobuild should fail when --image argument is empty" {
  export MAKE='echo'
  
  run dobuild --image 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--image" requires a non-empty option argument' ]]

  run dobuild --image= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ '"--image=" requires a non-empty option argument' ]]
}

@test "dobuild should print warning when --dockerfile argument is empty and fail while project kind is unknown" {
  export MAKE='echo'
  
  run dobuild -C "$TEMP" --dockerfile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ 'warning: "--dockerfile" ignored, requires a non-empty option argument' ]]
  [[ "$output" =~ 'error: unknown project kind' ]]

  run dobuild -C "$TEMP" --dockerfile= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ 'warning: "--dockerfile=" ignored, requires a non-empty option argument' ]]
  [[ "$output" =~ 'error: unknown project kind' ]]
}

@test "dobuild should pass --dockerfile argument to make as DOCKERFILE variable" {
  export MAKE='echo'
  
  run dobuild -f Makefile --dockerfile=Dockerfile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ 'DOCKERFILE=Dockerfile' ]]
}

@test "dobuild should try to find dockerfile by FILTER in project by default and fails while no exact match can be found" {
  export MAKE='echo'
  
  touch "$TEMP/Dockerfile"
  touch "$TEMP/arm32v7-alpine-x.dockerfile"
  touch "$TEMP/x86_64-alpine-x.dockerfile"
  
  run dobuild -C "$TEMP" -f Makefile 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ 'more than one match found for *.dockerfile in . using filter [^-]*-[^.]*' ]]
}

@test "dobuild should find exact match for dockerfile using MARCH and DISTRIB" {
  export MAKE='echo'
  
  touch "$TEMP/Dockerfile"
  touch "$TEMP/arm32v7-alpine-x.dockerfile"
  touch "$TEMP/x86_64-alpine-x.dockerfile"
  touch "$TEMP/arm32v7-ubuntu-x.dockerfile"
  touch "$TEMP/x86_64-ubuntu-x.dockerfile"
  
  run dobuild -C "$TEMP" -f Makefile MARCH=arm32v7 DISTRIB=ubuntu 2>&1
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ 'DOCKERFILE=./arm32v7-ubuntu-x.dockerfile' ]]
  
  run dobuild -C "$TEMP" -f Makefile MARCH=arm32v7 DISTRIB=alpine 2>&1
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ 'DOCKERFILE=./arm32v7-alpine-x.dockerfile' ]]
}

@test "dobuild should find exact match for dockerfile using FILTER" {
  export MAKE='echo'
  
  touch "$TEMP/Dockerfile"
  touch "$TEMP/arm32v7-alpine-x.dockerfile"
  touch "$TEMP/x86_64-alpine-x.dockerfile"
  touch "$TEMP/arm32v7-ubuntu-x.dockerfile"
  touch "$TEMP/x86_64-ubuntu-x.dockerfile"
  
  run dobuild -C "$TEMP" -f Makefile --filter 'arm.\?.\?v7-alpine-x' 2>&1
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ 'DOCKERFILE=./arm32v7-alpine-x.dockerfile' ]]
  [[ "$output" =~ 'FILTER=arm.\?.\?v7-alpine-x' ]]
  
  run dobuild -C "$TEMP" -f Makefile 'FILTER=arm.\?.\?v7-alpine-x' 2>&1
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ 'DOCKERFILE=./arm32v7-alpine-x.dockerfile' ]]
  [[ "$output" =~ 'FILTER=arm.\?.\?v7-alpine-x' ]]
}

@test "dobuild should fallback to Dockerfile in project by default" {
  export MAKE='echo'
  
  touch "$TEMP/Dockerfile"
  
  run dobuild -C "$TEMP" -f Makefile
  
  echo "$output"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "DOCKERFILE=./Dockerfile" ]]
}

@test "dobuild should print warning when --filter argument is empty and fail while project kind is unknown" {
  export MAKE='echo'
  
  run dobuild -C "$TEMP" --filter 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ 'warning: "--filter" ignored, requires a non-empty option argument' ]]
  [[ "$output" =~ 'error: unknown project kind' ]]
  
  run dobuild -C "$TEMP" --filter= 2>&1
  
  echo "$output"
  [ "$status" -ne 0 ]
  [[ "$output" =~ 'warning: "--filter=" ignored, requires a non-empty option argument' ]]
  [[ "$output" =~ 'error: unknown project kind' ]]
}


