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

running_in_background() {
  pid="$1"
  { "$PS" -o stat= -p "$pid" 2>/dev/null || echo '+'; } | grep -vq -e '+$'
}

PS="${PS:-ps}"

running_in_background "${@:-$$}"
