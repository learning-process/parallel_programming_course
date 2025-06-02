message(STATUS "Student's tasks")

project("parallel_programming_course")
set(exec_func_tests "ppc_func_tests")
set(exec_perf_tests "ppc_perf_tests")

# Init func tests executable files
set(list_of_exec_tests "")
if (USE_FUNC_TESTS)
    add_executable(${exec_func_tests} "${CMAKE_CURRENT_SOURCE_DIR}/runners/functional.cpp")
    list(APPEND list_of_exec_tests ${exec_func_tests})
endif (USE_FUNC_TESTS)

# Init perf tests executable files
if (USE_PERF_TESTS)
    add_executable(${exec_perf_tests} "${CMAKE_CURRENT_SOURCE_DIR}/runners/performance.cpp")
    list(APPEND list_of_exec_tests ${exec_perf_tests})
endif (USE_PERF_TESTS)

SUBDIRLIST(subdirs ${CMAKE_CURRENT_LIST_DIR})
foreach(subd ${subdirs})
    if (subd STREQUAL "runners")
        continue()
    endif ()
    add_subdirectory(${subd})
endforeach()

# Link 3rdparty libraries
add_library(stb_image INTERFACE)

foreach (exec_func ${list_of_exec_tests})
    target_link_libraries(${exec_func} PUBLIC Threads::Threads)
    target_link_libraries(${exec_func} PUBLIC ${OpenMP_libomp_LIBRARY})
    if( MPI_COMPILE_FLAGS )
        set_target_properties(${exec_func} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif( MPI_COMPILE_FLAGS )

    if( MPI_LINK_FLAGS )
        set_target_properties(${exec_func} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif( MPI_LINK_FLAGS )
    target_link_libraries(${exec_func} PUBLIC ${MPI_LIBRARIES})

    add_dependencies(${exec_func} ppc_onetbb)
    target_link_directories(${exec_func} PUBLIC ${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
    if(NOT MSVC)
        target_link_libraries(${exec_func} PUBLIC tbb)
    endif()

    target_link_directories(stb_image INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/stb)
    target_link_libraries(${exec_func} PUBLIC stb_image)

    add_dependencies(${exec_func} ppc_googletest)
    target_link_directories(${exec_func} PUBLIC "${CMAKE_BINARY_DIR}/ppc_googletest/install/lib")
    target_link_libraries(${exec_func} PUBLIC gtest gtest_main)
    enable_testing()
    add_test(NAME ${exec_func} COMMAND ${exec_func})

    # Install the executable
    install(TARGETS ${exec_func} RUNTIME DESTINATION bin)
endforeach ()

# Install the library
install(TARGETS ${name_lib} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

add_compile_definitions(PATH_TO_PPC_PROJECT="${CMAKE_SOURCE_DIR}")
