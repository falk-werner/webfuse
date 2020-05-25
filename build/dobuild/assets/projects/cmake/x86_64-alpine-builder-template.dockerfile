ARG REGISTRY_PREFIX=''
ARG DISTRIB_VERSION=3.9

FROM ${REGISTRY_PREFIX}alpine:${DISTRIB_VERSION} as builder

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

RUN set -x \
  && installdeps="tzdata" \
  && apk add --no-cache --virtual .install-deps $installdeps \
  && ls /usr/share/zoneinfo \
  && cp -H --remove-destination "/usr/share/zoneinfo/$TZ" /tmp/localtime \
  && apk del .install-deps \
  && mv /tmp/localtime /etc/localtime \
  && echo "$TZ" > /etc/timezone \
  && apk add --no-cache \
       bash \
       make \
       gcc \
       musl-dev

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG DUMB_INIT_VERSION=1.2.2
ARG DUMB_INIT_MTIME=
RUN set -x \
  && builddeps="vim" \
  && apk add --no-cache --virtual .build-deps $builddeps \
  && [ -n "$DUMB_INIT_MTIME" ] && export SOURCE_DATE_EPOCH="$DUMB_INIT_MTIME" \
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

FROM ${REGISTRY_PREFIX}alpine:${DISTRIB_VERSION}

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

COPY --from=builder /usr/local /usr/local
COPY --from=builder /etc/localtime /etc/localtime
COPY --from=builder /etc/timezone /etc/timezone

RUN set -x \
  && apk add --no-cache \
       coreutils \
       gcc \
       g++ \
       make \
       cmake \
       ninja \
       pkgconf \
       gdb \
       valgrind

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig

ENTRYPOINT ["dumb-init", "--"]
CMD [ "/bin/sh" ]

ONBUILD ARG USERID=1000
ONBUILD RUN set -x \
  && adduser -u "$USERID" -s /bin/bash -D user

# unused
ARG DISTRIB_ID=
ARG SYS=
ARG ABI=
ARG MARCH=
ARG HOSTMARCH=
ARG ID=
ARG VARIANT=
ARG QEMU_VERSION_=
