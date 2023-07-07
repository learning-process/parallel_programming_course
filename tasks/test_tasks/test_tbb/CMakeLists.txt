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
    elseif ()
        target_link_libraries(${ProjectId} gtest gtest_main tbb)
    endif ()

    enable_testing()
    add_test(NAME ${ProjectId} COMMAND ${ProjectId})

    if( UNIX )
        foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
            string(FIND ${SOURCE_FILE} ${PROJECT_BINARY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
            if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
                list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
            endif ()
        endforeach ()

        find_program(CPPCHECK_EXEC cppcheck)
        add_custom_target(
                "${ProjectId}_cppcheck" ALL
                COMMAND ${CPPCHECK_EXEC}
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
else( USE_TBB )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_TBB )
