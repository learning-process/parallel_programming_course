project( tbb_test )

message( STATUS "-- " ${PROJECT_NAME} )
add_executable( ${PROJECT_NAME} main.cpp )

if( WIN32 )
    file( GLOB TBB_PRE_LIB "${tbb_root}/lib/${SYSTEM_BIT}/vc14/*.lib" )
    target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS} ${TBB_PRE_LIB})
else( WIN32 )
    target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
endif( WIN32 )
