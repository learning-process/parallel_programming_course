include_directories(${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/external/fmt/include)

if(WIN32)
    set(PPC_FMT_FLAGS "/EHsc /DFMT_CONSTEVAL=inline")
else()
    set(PPC_FMT_FLAGS "-fexceptions -DFMT_CONSTEVAL=inline")
endif()

include(ExternalProject)
ExternalProject_Add(ppc_libenvpp
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/libenvpp"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/install"
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
        -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS=${PPC_FMT_FLAGS}
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_libenvpp/install"
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if(cmake_build_type_lower STREQUAL "debug")
    set(PPC_FMT_LIB_NAME fmtd)
else()
    set(PPC_FMT_LIB_NAME fmt)
endif()

if(WIN32)
    set(PPC_ENVPP_LIB_NAME libenvpp)
else()
    set(PPC_ENVPP_LIB_NAME envpp)
endif()
