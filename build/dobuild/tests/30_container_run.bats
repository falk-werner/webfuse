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
  IMAGE="${REGISTRY_PREFIX}docker:${DOCKER_VERSION}-dind"
  
  export IMAGE
}

@test "container_run has required deps docker" {
  "$DOCKER" --version
  echo "docker unreachable!?"
  echo "DOCKER_HOST=$DOCKER_HOST"
  "$DOCKER" info
  echo "docker failed to pull $IMAGE"
  "$DOCKER" pull "$IMAGE"
}

@test "container_run run command in a new container" {
  run container_run "$IMAGE" echo 'Hello world'
  echo "$output"
  [ "$status" -eq 0 ]
  [ "$output" = 'Hello world' ]
}

@test "container_run docker should be accessible by default" {
  run container_run "$IMAGE" docker version
  echo "DOCKER_HOST=$DOCKER_HOST"
  echo "$output"
  [ "$status" -eq 0 ]
}

@test "container_run docker should be inaccessible when DOCKER_HOST is set to empty" {
  DOCKER_HOST= run container_run "$IMAGE" docker version
  echo "$output"
  [ "$status" -ne 0 ]
}

@test "container_run run command with piped output" {
  OUTPUT="$(container_run "$IMAGE" echo 'Piped output' | cat)"
  echo "$OUTPUT"
  [ "$OUTPUT" = 'Piped output' ]
}

@test "container_run run command with piped input" {
  OUTPUT="$(echo 'Piped input' | container_run "$IMAGE" cat)"
  echo "$OUTPUT"
  [ "$OUTPUT" = 'Piped input' ]
}

@test "container_run delegates env vars starting with DOCKER_" {
  DOCKER_key=value run container_run "$IMAGE" printenv DOCKER_key
  echo "$output"
  [ "$status" -eq 0 ]
  [ "$output" = 'value' ]
}

@test "container_run doesn't delegate env var TZ" {
  TZ=UTC run container_run "$IMAGE" printenv TZ
  [ -z "$output" ]
}

@test "container_run delegates env var SOURCE_DATE_EPOCH" {
  SOURCE_DATE_EPOCH=1562027497 run container_run "$IMAGE" printenv SOURCE_DATE_EPOCH
  echo "$output"
  [ "$status" -eq 0 ]
  [ "$output" = '1562027497' ]
}

