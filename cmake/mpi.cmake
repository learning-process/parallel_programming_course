find_package( MPI )
if( MPI_FOUND )
    include_directories( ${MPI_INCLUDE_PATH} )
else( MPI_FOUND )
    message(FATAL_ERROR "MPI NOT FOUND")
endif( MPI_FOUND )
