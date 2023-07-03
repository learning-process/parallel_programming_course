# Build Core Boost components
SUBDIRLIST(subdirs ${CMAKE_SOURCE_DIR}/3rdparty/boost/libs)

foreach(subd ${subdirs})
        include_directories(${CMAKE_SOURCE_DIR}/3rdparty/boost/libs/${subd}/include)
endforeach()

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/boost)
if( USE_MPI )
        find_package( MPI )
        if( MPI_FOUND )
                add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/boost/libs/mpi)
        else( MPI_FOUND )
                set( USE_MPI OFF )
        endif( MPI_FOUND )
endif( USE_MPI )
