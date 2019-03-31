ARG CODENAME=bionic

FROM ubuntu:$CODENAME as builder

RUN set -x \
  && apt update \
  && apt upgrade -y \
  && apt install --yes --no-install-recommends \
       build-essential \
       cmake \
       ninja-build \
       pkg-config \
       rsync \
       gdb \
       gdbserver \
       valgrind \
  && rm -rf /var/lib/apt/lists/*

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG GTEST_VERSION=1.8.1

RUN set -x \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake "/usr/local/src/googletest-release-$GTEST_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf /tmp/out

ARG FUSE_VERSION=3.1.1

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       libtool \
       automake \
       gettext \
  && cd "/usr/local/src/libfuse-fuse-$FUSE_VERSION" \
  && ./makeconf.sh \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && "/usr/local/src/libfuse-fuse-$FUSE_VERSION/configure" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf /tmp/out \
  && rm -rf /var/lib/apt/lists/*

ARG WEBSOCKETS_VERSION=3.1.0

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       openssl \
       libssl-dev \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake "/usr/local/src/libwebsockets-$WEBSOCKETS_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf /tmp/out \
  && rm -rf /var/lib/apt/lists/*

ARG JANSSON_VERSION=2.12

RUN set -x \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake -DJANSSON_BUILD_DOCS=OFF "/usr/local/src/jansson-$JANSSON_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf /tmp/out

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

ARG USERID=1000

ARG PROJECT_ROOT=/workspace/src
ARG OUT=/workspace/out

RUN set -x \
  && useradd -u "$USERID" -ms /bin/bash user \
  && mkdir -p "$PROJECT_ROOT" "$OUT" \
  && chown user:user "$PROJECT_ROOT" "$OUT"

WORKDIR "$OUT"

