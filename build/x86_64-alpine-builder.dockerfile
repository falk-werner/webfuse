ARG REGISTRY_PREFIX=''
ARG DISTRIB_VERSION=3.9

FROM ${REGISTRY_PREFIX}alpine:${DISTRIB_VERSION} as builder

RUN set -x \
  && apk add --no-cache \
       bash \
       coreutils \
       gcc \
       g++ \
       make \
       python3 \
       py3-pip \
       py3-setuptools \
       py3-cryptography \
       cmake \
       ninja \
       pkgconf \
       tar \
       gdb \
       valgrind \
       git \
       ca-certificates \
       openssl \
       openssl-dev \
       linux-headers \
   && pip3 install meson

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG DUMB_INIT_VERSION=1.2.2

RUN set -x \
  && builddeps="vim" \
  && apk add --no-cache --virtual .build-deps $builddeps \
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
  && apk del .build-deps

ARG FUSE_VERSION=3.9.2

RUN set -x \
  && builddeps="eudev-dev" \
  && apk add --no-cache --virtual .build-deps $builddeps \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && meson -Dexamples=false "$builddir" "/usr/local/src/libfuse-fuse-$FUSE_VERSION" \
  && ninja "$PARALLELMFLAGS" -C "$builddir" install \
  && rm -rf "$builddir" \
  && apk del .build-deps

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig

ARG USERID=1000

RUN set -x \
  && adduser -u "$USERID" -s /bin/bash -D user

ENTRYPOINT ["dumb-init", "--"]
CMD [ "/bin/bash" ]
