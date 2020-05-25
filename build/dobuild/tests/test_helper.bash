#!/usr/bin/env bash
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

DOCKER="${DOCKER:-docker}"
export DOCKER

USERID="${USERID:-$(id -u)}"

_cancel() {
  if [[ -t 3 ]]; then
    exec 3<&-
  fi
}

debug_trace() {
  echo "${@}" 1>&2
  "${@}"
}

# workaround to avoid deadlocks on signal termination
trap _cancel SIGINT SIGTERM
