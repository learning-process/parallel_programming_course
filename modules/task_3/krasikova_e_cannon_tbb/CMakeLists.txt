set(PROJECT_NAME "krasikova_e_cannon_tbb")

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