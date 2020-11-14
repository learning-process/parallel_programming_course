DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"
# shellcheck disable=SC2164
mkdir "${DEPS_DIR}" && cd "${DEPS_DIR}"

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    travis_retry wget --no-check-certificate https://cmake.org/files/v3.18/cmake-3.18.0-Linux-x86_64.tar.gz
    tar -xvf cmake-3.18.0-Linux-x86_64.tar.gz > /dev/null
    mv cmake-3.18.0-Linux-x86_64 cmake-install
fi

PATH=${DEPS_DIR}/cmake-install:${DEPS_DIR}/cmake-install/bin:$PATH
# shellcheck disable=SC2164
cd "${TRAVIS_BUILD_DIR}"
