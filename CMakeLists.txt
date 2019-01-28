cmake_minimum_required( VERSION 2.8 )

message( STATUS "Parallel Programming Course" )

find_package( MPI REQUIRED )
if( MPI_FOUND )
    include_directories( ${MPI_INCLUDE_PATH} )
endif( MPI_FOUND )

if ( UNIX )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif( UNIX )

if ( MSVC )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
endif( MSVC )

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(NOT APPLE)
        set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp=libiomp5" )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp=libiomp5" )
    endif(NOT APPLE)
else(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_package( OpenMP REQUIRED )
    if ( OPENMP_FOUND )
        set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}" )
        set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
    endif( OPENMP_FOUND )
endif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

add_subdirectory(modules)
