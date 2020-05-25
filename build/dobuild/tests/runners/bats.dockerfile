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
ARG CODENAME=trusty

FROM ${REGISTRY_PREFIX}ubuntu:${CODENAME} as builder

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

ARG PARALLELMFLAGS=

ARG DOCKER_CHANNEL=stable
ARG DOCKER_VERSION=18.09.6
ARG DOCKER_MD5=a6be1e734421d05abfc4d3e28997e271
ARG DOCKER_DOWNLOAD=https://download.docker.com

ARG DOCKER_HOME="/usr/local/lib/docker-$DOCKER_VERSION"

ARG CAPATH="./ca-certificates"
COPY $CAPATH /usr/local/share/ca-certificates

RUN set -x \
  && { update-ca-certificates || true; } \
  && installdeps="tzdata" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $installdeps \
  && ls /usr/share/zoneinfo \
  && cp -H --remove-destination "/usr/share/zoneinfo/$TZ" /tmp/localtime \
  && { apt-get purge -y $installdeps || true; } \
  && mv /tmp/localtime /etc/localtime \
  && echo "$TZ" > /etc/timezone \
  && apt-get install --yes --no-install-recommends \
       ca-certificates \
       curl \
       openssl \
       build-essential \
       make \
  && rm -rf /var/lib/apt/lists/* \
  && update-ca-certificates

RUN set -x \
  && curl -fSL -s "$DOCKER_DOWNLOAD/linux/static/${DOCKER_CHANNEL}/$(uname -m)/docker-${DOCKER_VERSION}.tgz" -o docker.tgz \
  && md5sum docker.tgz \
  && echo "$DOCKER_MD5  docker.tgz" | md5sum -c - \
  && tar -xzvf docker.tgz \
  && mkdir -p "$DOCKER_HOME" \
  && mv docker/docker "$DOCKER_HOME" \
  && rm -rf docker \
  && rm docker.tgz \
  && ln -s "$DOCKER_HOME/docker" /usr/local/bin/docker

ARG BATS_VERSION=1.1.0
ARG BATS_MD5=0cb16021aa8f75a29240434c5aaae0a1
ARG BATS_DOWNLOAD=https://github.com/bats-core

RUN set -x \
  && builddeps="coreutils ncurses-bin bash" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $builddeps \
  && rm -rf /var/lib/apt/lists/* \
  && curl -fSL -s "$BATS_DOWNLOAD/bats-core/archive/v$BATS_VERSION.tar.gz" -o bats.tar.gz \
  && md5sum bats.tar.gz \
  && echo "$BATS_MD5  bats.tar.gz" | md5sum -c - \
  && tar -xzvf bats.tar.gz \
  && bash "bats-core-$BATS_VERSION/install.sh" /usr/local \
  && rm -rf "bats-core-$BATS_VERSION" \
  && rm bats.tar.gz

ARG DUMB_INIT_VERSION=1.2.2
ARG DUMB_INIT_MD5=6166084b05772cdcf615a762c6f3b32e
ARG DUMB_INIT_DOWNLOAD=https://github.com/Yelp/dumb-init

RUN set -x \
  && parallelMFlags="$PARALLELMFLAGS" \
  && [ -n "$parallelMFlags" ] || parallelMFlags="-j$(nproc)" \
  && builddeps="vim-common" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $builddeps \
  && rm -rf /var/lib/apt/lists/* \
  && curl -fSL -s "$DUMB_INIT_DOWNLOAD/archive/v${DUMB_INIT_VERSION}.tar.gz" -o dumb-init.tar.gz \
  && md5sum dumb-init.tar.gz \
  && echo "$DUMB_INIT_MD5  dumb-init.tar.gz" | md5sum -c - \
  && export SOURCE_DATE_EPOCH="$(stat -c '%Y' dumb-init.tar.gz)" \
  && tar -xzvf dumb-init.tar.gz \
  && cd "dumb-init-$DUMB_INIT_VERSION" \
  && make "$parallelMFlags" \
  && chmod +x dumb-init \
  && cp dumb-init /usr/local/bin/dumb-init \
  && cd .. \
  && rm -rf "dumb-init-$DUMB_INIT_VERSION" \
  && rm dumb-init.tar.gz \
  && apt-get purge -y $builddeps

ARG LIBFAKETIME_VERSION=0.9.7
ARG LIBFAKETIME_MD5=8617e2c6caf0977b3ce9a271f867302c
ARG LIBFAKETIME_DOWNLOAD=https://github.com/wolfcw/libfaketime

RUN set -x \
  && parallelMFlags="$PARALLELMFLAGS" \
  && [ -n "$parallelMFlags" ] || parallelMFlags="-j$(nproc)" \
  && curl -fSL -s "$LIBFAKETIME_DOWNLOAD/archive/v${LIBFAKETIME_VERSION}.tar.gz" -o libfaketime.tar.gz \
  && md5sum libfaketime.tar.gz \
  && echo "$LIBFAKETIME_MD5  libfaketime.tar.gz" | md5sum -c - \
  && export SOURCE_DATE_EPOCH="$(stat -c '%Y' libfaketime.tar.gz)" \
  && tar -xzvf libfaketime.tar.gz \
  && cd "libfaketime-$LIBFAKETIME_VERSION" \
  && prefix='/usr/local' \
  && make "$parallelMFlags" \
       CFLAGS="-fpic -lpthread -Wno-error -D'PREFIX=\"${prefix}\"' -D'LIBDIRNAME=\"lib\"'" \
       LDFLAGS='-fpic -lpthread' \
  && make install \
  && cd .. \
  && rm -rf "libfaketime-$LIBFAKETIME_VERSION" \
  && rm libfaketime.tar.gz

FROM ${REGISTRY_PREFIX}ubuntu:${CODENAME}

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

COPY --from=builder /usr/local /usr/local
COPY --from=builder /etc/localtime /etc/localtime
COPY --from=builder /etc/timezone /etc/timezone
COPY --from=builder /etc/apt/sources.list.d /etc/apt/sources.list.d

RUN set -x \
  && { update-ca-certificates || true; } \
  && apt-get update \
  && apt-get install --yes --no-install-recommends \
       coreutils \
       ncurses-bin \
       bash \
       procps \
       parallel \
       ca-certificates \
       make \
       git \
       curl \
  && rm -rf /var/lib/apt/lists/* \
  && update-ca-certificates \
  && useradd -ms /bin/bash user
  
USER user

# Accept the citation notice of GNU parallel (we aren't using this in a
# context where it make sense to cite GNU Parallel).
RUN set -x \
  && mkdir -p ~/.parallel \
  && touch ~/.parallel/will-cite

RUN dumb-init --version
RUN parallel --version
RUN docker --version
RUN bats --version

ENTRYPOINT ["dumb-init", "--"]
