set(PROJECT_NAME "khomenko_a_shell_sort_w_simple_merge_task3_tbb")

if ( USE_TBB )
    project( ${PROJECT_NAME} )

    message( STATUS "-- " ${PROJECT_NAME} )
    add_executable( ${PROJECT_NAME} main.cpp )

    if( WIN32 )
        target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS})
    else( WIN32 )
        target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
    endif( WIN32 )
else( USE_TBB )
    message( STATUS "-- ${PROJECT_NAME} - NOT BUILD!"  )
endif( USE_TBB )
