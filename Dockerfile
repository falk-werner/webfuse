FROM ubuntu:22.04 as builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y --no-install-recommends \
    locales \
    ca-certificates \
    git \
    build-essential \
    pkg-config \
    cmake \
    libfuse3-dev \
    libwebsockets-dev \
    libgtest-dev \
    libgmock-dev \
    clang-tidy \
    valgrind

FROM builder as devcontainer

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8  
ENV LANGUAGE en_US:en  
ENV LC_ALL en_US.UTF-8

ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME
RUN useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

FROM builder as buildrun

COPY . /src/webfuse
# workaround:
#   remote build directory, since --exclude is not available yet
#   .dockerignore is not suitable since we want build directory be present in DevContainer
RUN rm -rf /src/webfuse/build

WORKDIR /src/webfuse
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DWITHOUT_TEST=ON -DWITHOUT_CLANG_TIDY=ON
RUN cmake --build build
RUN DESTDIR=out cmake --install build

FROM scratch as build

COPY --from=buildrun /src/webfuse/out /