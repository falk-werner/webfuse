ARG REGISTRY_PREFIX=''
ARG BUILDER_TAG=0.0.1

ARG USERID=1000
ARG PARALLELMFLAGS=-j2
ARG HOSTMARCH=x86_64
ARG MARCH=$HOSTMARCH
ARG DISTRIB_ID=ubuntu
ARG DISTRIB_VERSION=bionic
ARG SYS=linux
ARG ABI=gnu

FROM ${REGISTRY_PREFIX}${HOSTMARCH}/${MARCH}/${DISTRIB_ID}/${DISTRIB_VERSION}/${SYS}/${ABI}/cmake/builder-template:${BUILDER_TAG}

COPY src /usr/local/src

ARG PARALLELMFLAGS=-j2

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

