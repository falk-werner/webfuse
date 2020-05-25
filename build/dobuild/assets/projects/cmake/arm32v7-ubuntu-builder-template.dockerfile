ARG REGISTRY_PREFIX=''
ARG DISTRIB_VERSION=bionic

FROM ${REGISTRY_PREFIX}arm32v7/ubuntu:${DISTRIB_VERSION} as builder

ARG QEMU_VERSION_=v3.1.0-2
COPY bin/qemu-arm-static-$QEMU_VERSION_ /usr/bin/qemu-arm-static

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

RUN set -x \
  && installdeps="tzdata" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $installdeps \
  && ls /usr/share/zoneinfo \
  && cp -H --remove-destination "/usr/share/zoneinfo/$TZ" /tmp/localtime \
  && { apt-get purge -y $installdeps || true; } \
  && mv /tmp/localtime /etc/localtime \
  && echo "$TZ" > /etc/timezone \
  && apt-get update \
  && apt-get install --yes --no-install-recommends \
       build-essential \
  && rm -rf /var/lib/apt/lists/*

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG DUMB_INIT_VERSION=1.2.2
ARG DUMB_INIT_MTIME=
RUN set -x \
  && builddeps="vim-common" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $builddeps \
  && rm -rf /var/lib/apt/lists/* \
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
  && apt-get purge -y $builddeps

FROM ${REGISTRY_PREFIX}arm32v7/ubuntu:${DISTRIB_VERSION}

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

COPY --from=builder /usr/bin/qemu-arm-static /usr/bin/qemu-arm-static
COPY --from=builder /usr/local /usr/local
COPY --from=builder /etc/localtime /etc/localtime
COPY --from=builder /etc/timezone /etc/timezone
COPY --from=builder /etc/apt/sources.list.d /etc/apt/sources.list.d

RUN set -x \
  && apt-get update \
  && apt-get install --yes --no-install-recommends \
       build-essential \
       cmake \
       ninja-build \
       pkg-config \
       gdb \
       gdbserver \
       valgrind \
  && rm -rf /var/lib/apt/lists/*

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

ENTRYPOINT ["dumb-init", "--"]
CMD [ "/bin/bash" ]

ONBUILD ARG USERID=1000
ONBUILD RUN set -x \
  && useradd -u "$USERID" -ms /bin/bash user

# unused
ARG DISTRIB_ID=
ARG SYS=
ARG ABI=
ARG MARCH=
ARG HOSTMARCH=
ARG ID=
ARG VARIANT=
