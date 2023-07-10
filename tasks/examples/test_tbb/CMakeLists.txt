get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_TBB )
    set(ProjectId "${ProjectId}_tbb")
    project( ${ProjectId} )
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${ALL_SOURCE_FILES} )

    if (APPLE)
        link_directories(${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
    endif (APPLE)

    add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )
    target_link_libraries(${ProjectId} ${PACK_LIB})

    if (APPLE)
        add_dependencies(${ProjectId} ppc_onetbb)
    endif (APPLE)

    target_link_libraries(${ProjectId} gtest gtest_main tbb)

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})

    CPPCHECK_AND_COUNTS_TESTS("${ProjectId}" "${ALL_SOURCE_FILES}")
else ( USE_TBB )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif ( USE_TBB )
