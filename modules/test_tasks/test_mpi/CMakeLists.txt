get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
enable_testing()

if( USE_MPI )
    project( ${ProjectId} )

    message( STATUS "-- " ${ProjectId} )
    add_executable( ${ProjectId} main.cpp )

    if( MPI_COMPILE_FLAGS )
        set_target_properties( ${ProjectId} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}" )
    endif( MPI_COMPILE_FLAGS )

    if( MPI_LINK_FLAGS )
        set_target_properties( ${ProjectId} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}" )
    endif( MPI_LINK_FLAGS )

    target_link_libraries( ${ProjectId} ${MPI_LIBRARIES} )
else( USE_MPI )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_MPI )
