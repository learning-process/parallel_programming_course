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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    endif( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
else( CMAKE_VERSION VERSION_LESS "3.1" )
    set( CMAKE_CXX_STANDARD 14 )
endif( CMAKE_VERSION VERSION_LESS "3.1" )

if( UNIX )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif( UNIX )

if( MSVC )
    option(CMAKE_EXE_LINKER_FLAGS  "" "${CMAKE_EXE_LINKER_FLAGS} /MD /MDd")
    option(CMAKE_C_FLAGS_RELEASE   "" "${CMAKE_C_FLAGS}          /WX /MD" )
    option(CMAKE_C_FLAGS_DEBUG     "" "${CMAKE_C_FLAGS}          /WX /MDd")
    option(CMAKE_C_FLAGS           "" "${CMAKE_C_FLAGS}          /WX /MD" )
    option(CMAKE_CXX_FLAGS_RELEASE "" "${CMAKE_CXX_FLAGS}        /WX /MD" )
    option(CMAKE_CXX_FLAGS_DEBUG   "" "${CMAKE_CXX_FLAGS}        /WX /MDd")
    option(CMAKE_CXX_FLAGS         "" "${CMAKE_CXX_FLAGS}        /WX /MD" )
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
