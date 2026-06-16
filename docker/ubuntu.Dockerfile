FROM ubuntu:26.04

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
    libomp-dev \
    gcc-15 g++-15 \
    gcovr zip \
 && wget -q https://apt.llvm.org/llvm.sh \
 && chmod +x llvm.sh \
 && ./llvm.sh 22 all \
 && rm llvm.sh \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-15 CXX=g++-15

CMD ["bash"]
