#!/bin/sh
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

set -e

container_id() {
  if [ "$#" -le 0 ]; then
    container_id_by_cgroup < /proc/self/cgroup
  else
    container_id_by_object "$@"
  fi
}

container_id_by_object() {
  "$DOCKER" inspect --format='{{.Id}}' "$@"
}

container_id_by_cgroup() {
  while IFS= read -r cmd; do
    id="$(echo "$cmd" | sed -n -e 's/[^:]*:[^:]*:.*\/\([0-9a-fA-F]\+\)$/\1/p')"
    if container_id_by_object "$id" >/dev/null 2>&1; then
      echo "$id"
      return 0
    fi
  done

  return 3
}

DOCKER="${DOCKER:-docker}"
container_id "$@"
