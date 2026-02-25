FROM ubuntu:24.04

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

ENV DEBIAN_FRONTEND=noninteractive

RUN set -e \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    ca-certificates curl wget gnupg lsb-release software-properties-common \
    python3 python3-pip \
    ninja-build cmake make \
    ccache \
    valgrind \
    libmpich-dev mpich \
    openmpi-bin openmpi-common libopenmpi-dev \
    gcc-14 g++-14 \
    gcovr zip \
 && wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor > /usr/share/keyrings/llvm-archive-keyring.gpg \
 && echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/noble/ llvm-toolchain-noble-22 main" \
    > /etc/apt/sources.list.d/llvm.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
    clang-22 clang-tools-22 clang-tidy-22 clang-format-22 \
    lldb-22 lld-22 llvm-22 llvm-22-dev \
    libomp5-22 libomp-22-dev \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-14 CXX=g++-14

CMD ["bash"]
