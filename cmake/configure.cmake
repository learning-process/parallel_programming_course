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

set( CMAKE_CXX_STANDARD 20 )

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
    set (COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -Werror")

    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMMON_COMPILER_FLAGS}")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS}")
    if (USE_COVERAGE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif(USE_COVERAGE)
endif()

if( MSVC )
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} /W4 /wd4267 /wd4244 /wd4100 /WX")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /W4 /wd4267 /wd4244 /wd4100 /WX")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /W4 /wd4267 /wd4244 /wd4100 /WX" )
endif( MSVC )
