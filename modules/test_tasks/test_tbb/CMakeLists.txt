get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_TBB )
    set(ProjectId "${ProjectId}_tbb")
    project( ${ProjectId} )
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE header_files "*.h")
    file(GLOB_RECURSE source_files "*.cpp")
    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${header_files} ${source_files})

    add_executable(${ProjectId} ${source_files})

    target_link_libraries(${ProjectId} ${PACK_LIB})
    if( WIN32 )
        target_link_libraries(${ProjectId} ${TBB_IMPORTED_TARGETS})
    else( WIN32 )
        target_link_libraries(${ProjectId} ${TBB_LIBRARIES})
    endif( WIN32 )
    target_link_libraries(${ProjectId} gtest gtest_main)

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})
else( USE_TBB )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_TBB )
