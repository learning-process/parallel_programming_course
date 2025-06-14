# Build Core OneTBB components
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/onetbb/include)

include(ExternalProject)

if(WIN32)
    set(ppc_onetbb_TEST_COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install/bin" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
    set(ppc_onetbb_TEST_COMMAND "")
endif()

ExternalProject_Add(ppc_onetbb
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/onetbb"
    PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb"
    BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build"
    INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install"
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/onetbb/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build/"
                      -DCMAKE_CXX_FLAGS="-w" -DCMAKE_C_FLAGS="-w"
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER} -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
                      -G${CMAKE_GENERATOR} -DTBB_TEST=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build" --config ${CMAKE_BUILD_TYPE} --parallel
    INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install"
    TEST_COMMAND      ${ppc_onetbb_TEST_COMMAND}
)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install/"
        DESTINATION "${CMAKE_INSTALL_PREFIX}")

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if(cmake_build_type_lower STREQUAL "debug")
    set(PPC_TBB_LIB_NAME tbb_debug)
else()
    set(PPC_TBB_LIB_NAME tbb)
endif()
