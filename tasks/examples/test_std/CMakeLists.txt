get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_STD )
    set(ProjectId "${ProjectId}_std")
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${ALL_SOURCE_FILES} )

    add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )

    target_link_libraries(${ProjectId} ${PACK_LIB} core_lib)
    target_link_libraries(${ProjectId} gtest gtest_main)
    target_link_libraries (${ProjectId} Threads::Threads)

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})

    CPPCHECK_AND_COUNTS_TESTS("${ProjectId}" "${ALL_SOURCE_FILES}")
else( USE_STD )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_STD )
