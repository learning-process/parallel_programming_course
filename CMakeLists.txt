cmake_minimum_required( VERSION 2.8 )

message( STATUS "Parallel Programming Course" )

if( CMAKE_VERSION VERSION_LESS "3.1" )
    if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
else( CMAKE_VERSION VERSION_LESS "3.1" )
    set( CMAKE_CXX_STANDARD 11 )
endif( CMAKE_VERSION VERSION_LESS "3.1" )

find_package( MPI )
if( MPI_FOUND )
    include_directories( ${MPI_INCLUDE_PATH} )
endif( MPI_FOUND )

find_package( OpenMP )
if( OpenMP_FOUND )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif( OpenMP_FOUND )

include( cmake/TBBGet.cmake )
set( TBB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/tbb_downloaded/tbb2019_20181203oss_win" )
tbb_get (TBB_ROOT tbb_root CONFIG_DIR TBB_DIR )
find_package( TBB )

if( TBB_FOUND )
    message(STATUS "Found TBB: TRUE (found version \"${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}\") ")
    include_directories( ${TBB_INCLUDE_DIRS} )
else( TBB_FOUND )
    message(STATUS "Not found TBB")
endif( TBB_FOUND )

if( UNIX )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif( UNIX )

if( MSVC )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
endif( MSVC )

add_subdirectory(modules)
