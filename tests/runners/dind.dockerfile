#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ARG REGISTRY_PREFIX=''
ARG DOCKER_VERSION=18.09.6

FROM ${REGISTRY_PREFIX}docker:${DOCKER_VERSION}-dind

ARG CAPATH="./ca-certificates"
COPY $CAPATH /usr/local/share/ca-certificates

RUN set -x \
  && { update-ca-certificates || true; } \
  && apk add --no-cache \
       ca-certificates \
  && update-ca-certificates

