cmake_minimum_required( VERSION 2.8 )

message( STATUS "Parallel Programming Course" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/arch" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/arch" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin" )

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

if( WIN32 )
    include( cmake/TBBGet.cmake )
    tbb_get( TBB_ROOT tbb_root RELEASE_TAG "2019_U2" CONFIG_DIR TBB_DIR )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SYSTEM_BIT "intel64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SYSTEM_BIT "ia32")
    endif()

    file( GLOB TBB_PRE_LIB "${tbb_root}/lib/${SYSTEM_BIT}/vc14/*.lib" )
    file( COPY ${TBB_PRE_LIB} DESTINATION "${CMAKE_BINARY_DIR}/bin")

    find_package( TBB )
else( WIN32 )
    include( cmake/FindTBB.cmake )
endif( WIN32 )

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
