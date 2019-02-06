project( tbb )

message( STATUS "-- " ${PROJECT_NAME} )
add_executable( ${PROJECT_NAME} main.cpp )

if( WIN32 )
    target_link_libraries(tbb ${TBB_IMPORTED_TARGETS})
else( WIN32 )
    target_link_libraries(tbb ${TBB_LIBRARIES})
endif( WIN32 )
