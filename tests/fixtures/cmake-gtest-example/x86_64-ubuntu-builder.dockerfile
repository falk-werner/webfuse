ARG REGISTRY_PREFIX=''
ARG CODENAME=bionic

FROM ${REGISTRY_PREFIX}ubuntu:${CODENAME} as builder

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

RUN set -x \
  && installdeps="tzdata" \
  && rm -f /etc/apt/sources.list.d/*.list \
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
       cmake \
  && rm -rf /var/lib/apt/lists/*

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

ARG DUMB_INIT_VERSION=1.2.2
ARG DUMB_INIT_MTIME=
RUN set -x \
  && builddeps="vim-common" \
  && apt-get update \
  && apt-get install --yes --no-install-recommends $builddeps  \
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

ARG GTEST_VERSION=1.8.1
ARG GTEST_MTIME=

RUN set -x \
  && [ -n "$GTEST_MTIME" ] && export SOURCE_DATE_EPOCH="$GTEST_MTIME" \
  && builddir="/tmp/out" \
  && mkdir -p "$builddir" \
  && cd "$builddir" \
  && cmake "/usr/local/src/googletest-release-$GTEST_VERSION" \
  && make "$PARALLELMFLAGS" install \
  && rm -rf "$builddir"

FROM ${REGISTRY_PREFIX}ubuntu:${CODENAME}

ARG TZ=UTC
ENV LANG=C.UTF-8
ENV LC_ALL=${LANG}

COPY --from=builder /usr/local /usr/local
COPY --from=builder /etc/localtime /etc/localtime
COPY --from=builder /etc/timezone /etc/timezone
COPY --from=builder /etc/apt/sources.list.d /etc/apt/sources.list.d

RUN set -x \
  && rm -f /etc/apt/sources.list.d/*.list \
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

ARG USERID=1000
RUN set -x \
  && useradd -u "$USERID" -ms /bin/bash user

ENTRYPOINT ["dumb-init", "--"]
CMD [ "/bin/bash" ]

