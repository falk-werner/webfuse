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

print_cgroup_container() {
  cat <<EOF
10:perf_event:/
9:devices:/docker/$1
8:memory:/docker/$1
7:cpu,cpuacct:/docker/$1
6:freezer:/docker/$1
5:cpuset:/docker/$1
4:pids:/docker/$1
3:blkio:/docker/$1
2:net_cls,net_prio:/docker/$1
1:name=systemd:/docker/$1
EOF
}

print_cgroup_host() {
  cat <<EOF
10:perf_event:/
9:devices:/user.slice
8:memory:/user.slice
7:cpu,cpuacct:/user.slice
6:freezer:/
5:cpuset:/
4:pids:/user.slice/user-1000.slice/session-2.scope
3:blkio:/user.slice
2:net_cls,net_prio:/
1:name=systemd:/user.slice/user-1000.slice/session-2.scope
EOF
}

@test "get_container_id of known id" {
  source get_container_id.sh || true
  
  container_id_by_object() {
    return 0
  }
  
  container_id 1234
}

@test "get_container_id of unknown id" {
  source get_container_id.sh || true
  
  container_id_by_object() {
    return 3
  }
  
  run container_id 1234
  echo "$output"
  [ "$status" -ne 0 ]  
  [ -z "$output" ]
}

@test "get_container_id self-id by cgroup" {
  source get_container_id.sh || true
  
  container_id_by_object() {
    if [ "$1" = '8422b85b7e25dbe4cbc79f4d76392c34b2da4529ef3bc104ad2e097d44859d58' ]; then
      echo "$1"
    else
      return 3
    fi
  }

  container_id_by_cgroup < <(print_cgroup_container 8422b85b7e25dbe4cbc79f4d76392c34b2da4529ef3bc104ad2e097d44859d58)
  
  run container_id_by_cgroup < <(print_cgroup_container 3a6630ef0bfe2ab9b296d66aef9a29beecf764e03cce61289d08e888f5015cbe)
  echo "$output"
  [ "$status" -ne 0 ]
  [ -z "$output" ]
  
  run container_id_by_cgroup < <(print_cgroup_host)
  echo "$output"
  [ "$status" -ne 0 ]  
  [ -z "$output" ]
}

