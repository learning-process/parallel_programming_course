include_directories(${CMAKE_SOURCE_DIR}/3rdparty/json/include)
include(ExternalProject)
ExternalProject_Add(ppc_json
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/json"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_json"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/json/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build/"
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -D CMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER} -D CMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/install")
