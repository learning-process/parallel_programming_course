# Build googletest components
if(NOT USE_SEQ AND NOT USE_MPI AND NOT USE_OMP AND NOT USE_TBB AND NOT USE_STL)
    return()
endif()

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/googletest/googletest/include)
include(ExternalProject)
ExternalProject_Add(ppc_googletest
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/googletest"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/googletest/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build/"
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -D CMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER} -D CMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER} -Dgtest_force_shared_crt=ON -DCMAKE_CXX_FLAGS="-w" -DCMAKE_C_FLAGS="-w"
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install")
