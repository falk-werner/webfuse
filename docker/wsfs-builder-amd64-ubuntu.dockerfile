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
  && rm -rf /var/lib/apt/lists/*

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

RUN set -x \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake /usr/local/src/googletest-release-* \
  && make $PARALLELMFLAGS install \
  && rm -rf /tmp/out

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       libtool \
       automake \
       gettext \
  && cd /usr/local/src/libfuse-fuse-* \
  && ./makeconf.sh \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && /usr/local/src/libfuse-fuse-*/configure \
  && make $PARALLELMFLAGS install \
  && rm -rf /tmp/out \
  && rm -rf /var/lib/apt/lists/*

RUN set -x \
  && apt update \
  && apt install --yes --no-install-recommends \
       openssl \
       libssl-dev \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake /usr/local/src/libwebsockets-* \
  && make $PARALLELMFLAGS install \
  && rm -rf /tmp/out \
  && rm -rf /var/lib/apt/lists/*

RUN set -x \
  && mkdir -p /tmp/out \
  && cd /tmp/out \
  && cmake -DJANSSON_BUILD_DOCS=OFF /usr/local/src/jansson-* \
  && make $PARALLELMFLAGS install \
  && rm -rf /tmp/out

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

ARG USERID=1000

RUN useradd -u $USERID -ms /bin/bash user


