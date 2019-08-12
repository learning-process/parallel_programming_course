get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
enable_testing()

if ( USE_TBB )
    project( ${ProjectId} )

    message( STATUS "-- " ${ProjectId} )
    add_executable( ${ProjectId} main.cpp )

    if( WIN32 )
        target_link_libraries(${ProjectId} ${TBB_IMPORTED_TARGETS})
    else( WIN32 )
        target_link_libraries(${ProjectId} ${TBB_LIBRARIES})
    endif( WIN32 )
else( USE_TBB )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_TBB )
