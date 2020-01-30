get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_STD )
    set(ProjectId "${ProjectId}_std")
    project( ${ProjectId} )
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE header_files "*.h")
    file(GLOB_RECURSE source_files "*.cpp")
    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${header_files} ${source_files})

    add_executable(${ProjectId} ${source_files})

    target_link_libraries(${ProjectId} ${PACK_LIB})
    target_link_libraries(${ProjectId} gtest gtest_main)
    target_link_libraries (${ProjectId} Threads::Threads)

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})
else( USE_STD )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_STD )
