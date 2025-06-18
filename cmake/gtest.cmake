include_directories(${CMAKE_SOURCE_DIR}/3rdparty/googletest/googletest/include)
include(ExternalProject)

ExternalProject_Add(ppc_googletest
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/googletest"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install"

        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
        -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_FLAGS=-w
        -DCMAKE_CXX_FLAGS=-w
        -Dgtest_force_shared_crt=ON

        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install"
)
