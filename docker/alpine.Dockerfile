FROM alpine:3.23

RUN set -eux; \
  apk add --no-cache \
    bash \
    build-base \
    ca-certificates \
    ccache \
    clang \
    clang-extra-tools \
    cmake \
    curl \
    git \
    gnupg \
    lld \
    llvm \
    make \
    ninja \
    openmpi \
    openmpi-dev \
    python3 \
    py3-pip \
    valgrind \
    zip

ENV CC=gcc CXX=g++

CMD ["bash"]
