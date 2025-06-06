include_directories(${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/external/fmt/include)

include(ExternalProject)
ExternalProject_Add(ppc_libenvpp
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/libenvpp"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build/"
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -D CMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER} -D CMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/install")
