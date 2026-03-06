FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ARG LLVM_VER=22.1.0
ARG TARGETARCH

RUN set -e \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    ca-certificates curl gnupg lsb-release software-properties-common \
    python3 python3-pip \
    ninja-build cmake make \
    ccache \
    valgrind \
    libmpich-dev mpich \
    openmpi-bin openmpi-common libopenmpi-dev \
    libomp-dev \
    gcc-14 g++-14 \
    gcovr zip \
    xz-utils \
 && case "${TARGETARCH}" in \
      amd64) llvm_pkg_arch="X64" ;; \
      arm64) llvm_pkg_arch="ARM64" ;; \
      *) echo "Unsupported TARGETARCH: ${TARGETARCH}" >&2; exit 1 ;; \
    esac \
 && llvm_major="${LLVM_VER%%.*}" \
 && llvm_pkg="LLVM-${LLVM_VER}-Linux-${llvm_pkg_arch}" \
 && curl -fsSL --retry 5 --retry-delay 2 --retry-all-errors \
      -o "${llvm_pkg}.tar.xz" \
      "https://github.com/llvm/llvm-project/releases/download/llvmorg-${LLVM_VER}/${llvm_pkg}.tar.xz" \
 && tar -xf "${llvm_pkg}.tar.xz" -C /opt \
 && ln -s "/opt/${llvm_pkg}" "/opt/llvm-${llvm_major}" \
 && ln -s "/opt/llvm-${llvm_major}" /opt/llvm \
 && ln -s /opt/llvm/bin/clang++ /opt/llvm/bin/clang++-"${llvm_major}" \
 && ln -s /opt/llvm/bin/clang-tidy /opt/llvm/bin/clang-tidy-"${llvm_major}" \
 && ln -s /opt/llvm/bin/clang-format /opt/llvm/bin/clang-format-"${llvm_major}" \
 && rm "${llvm_pkg}.tar.xz" \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-14 CXX=g++-14
ENV PATH="/opt/llvm/bin:${PATH}"

CMD ["bash"]
