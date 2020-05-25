ARG REGISTRY_PREFIX=''
ARG SYS
ARG ABI
ARG DISTRIB_ID
ARG DISTRIB_VERSION

FROM ${REGISTRY_PREFIX}oracle/${DISTRIB_ID}:${DISTRIB_VERSION}-${ABI}

RUN set -x \
  && gu install native-image

ARG ID
ARG GRADLE_HOME=/usr/local/lib/gradle-${ID}
COPY bin/gradle-${ID} $GRADLE_HOME
ENV PATH=$PATH:$GRADLE_HOME/bin

RUN set -x \
  && java -version \
  && javac -version \
  && native-image --version \
  && gradle --version

ARG TZ=UTC
ENV LANG=C
ENV LC_ALL=${LANG}

CMD [ "/bin/bash" ]

# unused
ARG PARALLELMFLAGS=
ARG MARCH=
ARG HOSTMARCH=
ARG VARIANT=
