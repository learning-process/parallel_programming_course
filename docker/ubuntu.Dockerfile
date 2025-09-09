FROM ubuntu:24.04

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
    libomp-dev \
    gcc-14 g++-14 \
    gcovr zip \
 && wget -q https://apt.llvm.org/llvm.sh \
 && chmod +x llvm.sh \
 && ./llvm.sh 21 all \
 && rm llvm.sh \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-14 CXX=g++-14

CMD ["bash"]
