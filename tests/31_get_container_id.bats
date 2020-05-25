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

build_and_run() {
  debug_trace "$DOCKER" build --rm --iidfile "$IDDFILE" --file "$DOCKERFILE" \
    --build-arg "DOCKER_VERSION=$DOCKER_VERSION" \
    --build-arg "REGISTRY_PREFIX=$REGISTRY_PREFIX" \
    "$PROJECTPATH" 1>&2

  set -- "$(cat "$IDDFILE")" "$@"
  
  debug_trace container_run "$@"
}

setup() {
  PROJECTPATH="$(readlink -f "$BATS_TEST_DIRNAME/..")"
  BINPATH="$PROJECTPATH/bin"
  PATH="$BINPATH:$PATH"
  IDDFILE="$(mktemp --tmpdir="$BATS_TMPDIR" idd_XXXXXXXXXX.txt)"
  DOCKERFILE="$BATS_TEST_DIRNAME/get_container_id.dockerfile"
  
  export PROJECTPATH
  export IDDFILE
  export DOCKERFILE
}

teardown() {
  "$DOCKER" rmi "$(cat "$IDDFILE")"
  rm -f "$IDDFILE"
}

@test "get_container_id has required dep docker" {
  "$DOCKER" --version
  echo "docker unreachable!?"
  echo "DOCKER_HOST=$DOCKER_HOST"
  "$DOCKER" info
}

@test "get_container_id self-id within container" {
  ID="$(build_and_run ./get_container_id.sh)"
  [ -n "$ID" ]
}
