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

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

    set(PACK_LIB "${ProjectId}_lib")
    add_library(${PACK_LIB} STATIC ${ALL_SOURCE_FILES} )

    add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )

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

    if( UNIX )
        foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
            string(FIND ${SOURCE_FILE} ${PROJECT_BINARY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
            if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
                list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
            endif ()
        endforeach ()

        find_program(CPPCHECK cppcheck)
        add_custom_target(
                "${ProjectId}_cppcheck" ALL
                COMMAND ${CPPCHECK}
                --enable=warning,performance,portability,information,missingInclude
                --language=c++
                --std=c++11
                --error-exitcode=1
                --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
                --verbose
                --quiet
                ${ALL_SOURCE_FILES}
        )
    endif( UNIX )

    SET(ARGS_FOR_CHECK_COUNT_TESTS "")
    foreach (FILE_ELEM ${ALL_SOURCE_FILES})
        set(ARGS_FOR_CHECK_COUNT_TESTS "${ARGS_FOR_CHECK_COUNT_TESTS} ${FILE_ELEM}")
    endforeach ()

    add_custom_target("${ProjectId}_check_count_tests" ALL
            COMMAND "${Python3_EXECUTABLE}"
                ${CMAKE_SOURCE_DIR}/scripts/check_count_tests.py
                ${ProjectId}
                ${ARGS_FOR_CHECK_COUNT_TESTS}
    )
else( USE_MPI )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_MPI )
