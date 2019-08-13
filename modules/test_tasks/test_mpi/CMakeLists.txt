get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
enable_testing()

if( USE_MPI )
    if( UNIX )
        set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-uninitialized")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-uninitialized")
    endif( UNIX )

    set(ProjectId "${ProjectId}_mpi")
    project( ${ProjectId} )
    message( STATUS "-- " ${ProjectId} )

    file(GLOB_RECURSE header_files "*.h")
    file(GLOB_RECURSE source_files "*.cpp")
    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${header_files} ${source_files})

    add_executable( ${ProjectId} ${source_files} )

    target_link_libraries(${ProjectId} ${PACK_LIB})
    if( MPI_COMPILE_FLAGS )
        set_target_properties( ${ProjectId} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}" )
    endif( MPI_COMPILE_FLAGS )

    if( MPI_LINK_FLAGS )
        set_target_properties( ${ProjectId} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}" )
    endif( MPI_LINK_FLAGS )
    target_link_libraries( ${ProjectId} ${MPI_LIBRARIES} )
    target_link_libraries(${ProjectId} gtest gtest_main)

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})
else( USE_MPI )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_MPI )
