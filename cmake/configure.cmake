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

if( CMAKE_VERSION VERSION_LESS "3.1" )
    if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++20")
    endif( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
else( CMAKE_VERSION VERSION_LESS "3.1" )
    set( CMAKE_CXX_STANDARD 20 )
endif( CMAKE_VERSION VERSION_LESS "3.1" )

#if( UNIX )
#    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
#    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
#endif( UNIX )

if( MSVC )
    option(CMAKE_EXE_LINKER_FLAGS  "" "${CMAKE_EXE_LINKER_FLAGS} /MT /MTd")
    option(CMAKE_C_FLAGS_RELEASE   "" "${CMAKE_C_FLAGS}          /WX /MT" )
    option(CMAKE_C_FLAGS_DEBUG     "" "${CMAKE_C_FLAGS}          /WX /MTd")
    option(CMAKE_C_FLAGS           "" "${CMAKE_C_FLAGS}          /WX /MT" )
    option(CMAKE_CXX_FLAGS_RELEASE "" "${CMAKE_CXX_FLAGS}        /WX /MT" )
    option(CMAKE_CXX_FLAGS_DEBUG   "" "${CMAKE_CXX_FLAGS}        /WX /MTd")
    option(CMAKE_CXX_FLAGS         "" "${CMAKE_CXX_FLAGS}        /WX /MT" )
endif( MSVC )

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

MACRO(CPPCHECK_TEST ProjectId ALL_SOURCE_FILES)
    if( UNIX )
        foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
            string(FIND ${SOURCE_FILE} ${PROJECT_BINARY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
            if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
                list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
            endif ()
        endforeach ()
        if (NOT APPLE)
            find_program(CPPCHECK_EXEC /usr/bin/cppcheck)
            add_custom_target(
                    "${ProjectId}_cppcheck" ALL
                    COMMAND ${CPPCHECK_EXEC}
                    --enable=warning,performance,portability,information
                    --language=c++
                    --std=c++11
                    --error-exitcode=1
                    --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
                    --verbose
                    --quiet
                    ${ALL_SOURCE_FILES}
            )
        ENDIF ()
    endif( UNIX )
ENDMACRO()
