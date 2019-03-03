ARG CODENAME=bionic

FROM arm32v7/ubuntu:$CODENAME

COPY qemu-arm-static /usr/bin/

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       openssl \
       ca-certificates \
       curl \
       build-essential \
       cmake \
       ninja-build \
       pkg-config

ARG NPROC=1
ARG GTEST_VERSION=1.8.1

RUN set -x \
  && curl -fSL https://github.com/google/googletest/archive/release-$GTEST_VERSION.tar.gz -o /tmp/gtest-$GTEST_VERSION.tar.gz \
  && tar -C /tmp -xf /tmp/gtest-$GTEST_VERSION.tar.gz \
  && (   cd /tmp/googletest-release-$GTEST_VERSION \
      && cmake . \
      && make -j$NPROC install) \
  && rm /tmp/gtest-$GTEST_VERSION.tar.gz \
  && rm -rf /tmp/googletest-release-$GTEST_VERSION

ARG FUSE_VERSION=3.1.1

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       libtool \
       automake \
       gettext \
  && curl -fSL https://github.com/libfuse/libfuse/archive/fuse-$FUSE_VERSION.tar.gz -o /tmp/fuse-$FUSE_VERSION.tar.gz \
  && tar -C /tmp -xf /tmp/fuse-$FUSE_VERSION.tar.gz \
  && (   cd /tmp/libfuse-fuse-$FUSE_VERSION \
      && ./makeconf.sh \
      && ./configure \
      && make -j$NPROC install) \
  && rm /tmp/fuse-$FUSE_VERSION.tar.gz \
  && rm -rf /tmp/libfuse-fuse-$FUSE_VERSION

ARG WEBSOCKETS_VERSION=3.1.0

RUN set -x \
  && apt install --yes --no-install-recommends \
       libssl-dev \
  && curl -fSL https://github.com/warmcat/libwebsockets/archive/v$WEBSOCKETS_VERSION.tar.gz -o /tmp/libwebsockets-$WEBSOCKETS_VERSION.tar.gz \
  && tar -C /tmp -xf /tmp/libwebsockets-$WEBSOCKETS_VERSION.tar.gz \
  && (   cd /tmp/libwebsockets-$WEBSOCKETS_VERSION \
      && cmake . \
      && make -j$NPROC install) \
  && rm /tmp/libwebsockets-$WEBSOCKETS_VERSION.tar.gz \
  && rm -rf /tmp/libwebsockets-$WEBSOCKETS_VERSION

ARG JANSSON_VERSION=2.12

RUN set -x \
  && curl -fSL https://github.com/akheron/jansson/archive/v$JANSSON_VERSION.tar.gz -o /tmp/libjansson-$JANSSON_VERSION.tar.gz \
  && tar -C /tmp -xf /tmp/libjansson-$JANSSON_VERSION.tar.gz \
  && (   cd /tmp/jansson-$JANSSON_VERSION \
      && cmake . \
      && make -j$NPROC install) \
  && rm /tmp/libjansson-$JANSSON_VERSION.tar.gz \
  && rm -rf /tmp/jansson-$JANSSON_VERSION

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

RUN useradd -ms /bin/bash test


