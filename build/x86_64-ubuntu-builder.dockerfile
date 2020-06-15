ARG REGISTRY_PREFIX=''
ARG DISTRIB_VERSION=bionic

FROM ${REGISTRY_PREFIX}ubuntu:${DISTRIB_VERSION} as builder

RUN set -x \
  && apt update \
  && apt upgrade -y \
  && apt install --yes --no-install-recommends \
       build-essential \
       python3 \
       python3-pip \
       python3-setuptools \
       python3-wheel \
       cmake \
       ninja-build \
       pkg-config \
       gdb \
       gdbserver \
       valgrind \
       lcov \
       git \
       ca-certificates \
       openssl \
       libssl-dev \
   && pip3 install --system meson

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

ARG FUSE_VERSION=3.9.2

RUN set -x \
  && builddeps="udev gettext" \
  && apt install --yes --no-install-recommends $builddeps \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && meson -Dexamples=false "$builddir" "/usr/local/src/libfuse-fuse-$FUSE_VERSION" \
  && ninja "$PARALLELMFLAGS" -C "$builddir" install \
  && rm -rf "$builddir" \
  && apt purge -y $builddeps

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

ARG USERID=1000

RUN set -x \
  && useradd -u "$USERID" -ms /bin/bash user

ENTRYPOINT ["dumb-init", "--"]
CMD [ "/bin/bash" ]

