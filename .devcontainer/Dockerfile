FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y --no-install-recommends \
    ca-certificates \
    git \
    openssh-client \
    build-essential \
    pkg-config \
    cmake \
    clang-tidy \
    valgrind \
    libgtest-dev \
    libgmock-dev \
    libfuse3-dev \
    libwebsockets-dev

ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME
RUN useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

USER $USERNAME
