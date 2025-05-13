if(MSVC)
    option(gtest_force_shared_crt "" TRUE)
endif(MSVC)

if (APPLE)
    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG TRUE)
endif(APPLE)

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/arch" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/arch" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/arch" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

if ( MSVC AND (CMAKE_C_COMPILER_ID STREQUAL "Clang") AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set( CMAKE_CXX_STANDARD 17 )
else ()
    set( CMAKE_CXX_STANDARD 20 )
endif ()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_SKIP_BUILD_RPATH OFF)

if( UNIX )
    set(COMMON_COMPILER_FLAGS
        "${COMMON_COMPILER_FLAGS}            \
        -Wall -Wextra                        \
        -Wno-unused-parameter                \
        -Wno-cast-function-type              \
        -Wsign-compare")
    if (NOT APPLE)
        set(COMMON_COMPILER_FLAGS
            "${COMMON_COMPILER_FLAGS}        \
            -Wpedantic                       \
            -Wpointer-arith                  \
            -Wcast-align                     \
            -Wwrite-strings                  \
            -Wswitch-enum                    \
            -Wnull-dereference               \
            -Wswitch-enum                    \
            -Wformat=2                       \
            -Wmissing-declarations           \
            -Wno-c11-extensions")
    endif (NOT APPLE)
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS}                    \
        -Wold-style-definition               \
        -Wmissing-prototypes")

    if ("${ENABLE_ADDRESS_SANITIZER}" OR "${ENABLE_UB_SANITIZER}")
        set (COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -Wno-cast-align")
    endif()

    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMMON_COMPILER_FLAGS}")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS}")
    if (USE_COVERAGE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif(USE_COVERAGE)
endif()

if( MSVC )
    set(COMMON_FLAGS "/W4 /wd4267 /wd4244")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${COMMON_FLAGS}" )
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} ${COMMON_FLAGS}")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMMON_FLAGS}" )
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_FLAGS}")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" )
endif( MSVC )
