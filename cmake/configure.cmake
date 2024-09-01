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
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wsign-compare -Werror")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wsign-compare -Werror")
    if (USE_COVERAGE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif(USE_COVERAGE)
endif( UNIX )

if( MSVC )
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} /W4 /wd4267 /wd4244 /wd4100 /WX")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /wd4267 /wd4244 /wd4100 /WX" )
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /W4 /wd4267 /wd4244 /wd4100 /WX")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /W4 /wd4267 /wd4244 /wd4100 /WX" )
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
    if( UNIX AND USE_CPPCHECK)
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
                    --suppress=missingInclude
                    --error-exitcode=1
                    --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
                    --verbose
                    --quiet
                    ${ALL_SOURCE_FILES}
            )
        ENDIF ()
    endif( UNIX )
ENDMACRO()
