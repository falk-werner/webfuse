ARG REGISTRY_PREFIX=''
ARG CODENAME=testing-slim

FROM ${REGISTRY_PREFIX}debian:${CODENAME} as builder

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
       valgrind

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG DUMB_INIT_VERSION=1.2.2

RUN set -x \
  && builddeps="xxd" \
  && apt install --yes --no-install-recommends $builddeps \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && cp -R "/usr/local/src/dumb-init-$DUMB_INIT_VERSION" . \
  && cd "dumb-init-$DUMB_INIT_VERSION" \
  && make "$PARALLELMFLAGS" \
  && chmod +x dumb-init \
  && mv dumb-init /usr/local/bin/dumb-init \
  && dumb-init --version \
  && rm -rf "$builddir" \
  && apt purge -y $builddeps

ARG GTEST_VERSION=1.8.1

RUN set -x \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && cmake "/usr/local/src/googletest-release-$GTEST_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf "$builddir"

ARG FUSE_VERSION=3.1.1

RUN set -x \
  && builddeps="libtool automake gettext" \
  && apt install --yes --no-install-recommends $builddeps \
  && cd "/usr/local/src/libfuse-fuse-$FUSE_VERSION" \
  && ./makeconf.sh \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && "/usr/local/src/libfuse-fuse-$FUSE_VERSION/configure" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf "$builddir" \
  && apt purge -y $builddeps

ARG WEBSOCKETS_VERSION=3.1.0

RUN set -x \
  && apt install --yes --no-install-recommends \
       ca-certificates \
       openssl \
       libssl-dev \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && cmake "/usr/local/src/libwebsockets-$WEBSOCKETS_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf "$builddir"

ARG JANSSON_VERSION=2.12

RUN set -x \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && cmake -DJANSSON_BUILD_DOCS=OFF "/usr/local/src/jansson-$JANSSON_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf "$builddir"

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

ARG USERID=1000

ARG PROJECT_ROOT=/workspace/src
ARG OUT=/workspace/out

RUN set -x \
  && useradd -u "$USERID" -ms /bin/bash user \
  && mkdir -p "$PROJECT_ROOT" "$OUT" \
  && chown user:user "$PROJECT_ROOT" "$OUT"

WORKDIR "$OUT"

ENTRYPOINT ["dumb-init", "--"]

