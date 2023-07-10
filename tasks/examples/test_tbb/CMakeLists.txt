get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_TBB )
    set(ProjectId "${ProjectId}_tbb")
    project( ${ProjectId} )
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${ALL_SOURCE_FILES} )

    add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )

    target_link_libraries(${ProjectId} ${PACK_LIB})
    if (APPLE)
        add_dependencies(${ProjectId} ppc_onetbb)
        target_link_libraries(${ProjectId} gtest gtest_main)

        file(GLOB TBB_APPLE_LIBS "${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib/*.dylib")

        foreach (tbb_lib ${TBB_APPLE_LIBS})
            target_link_libraries(${ProjectId} ${tbb_lib})
            configure_file(${tbb_lib} "${CMAKE_BINARY_DIR}/bin" COPYONLY)
        endforeach ()
    else ()
        target_link_libraries(${ProjectId} gtest gtest_main tbb)
    endif ()

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})

    CPPCHECK_AND_COUNTS_TESTS(${ProjectId} ${PROJECT_BINARY_DIR} ${ALL_SOURCE_FILES})
else( USE_TBB )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_TBB )
