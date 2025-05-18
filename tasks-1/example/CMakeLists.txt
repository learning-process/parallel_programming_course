# Print student task name
get_filename_component(TASK_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
message(STATUS "-- ${TASK_NAME}")

# Init project
project(${TASK_NAME})
set(exec_func_tests "${TASK_NAME}_func_tests")
set(exec_perf_tests "${TASK_NAME}_perf_tests")
set(test_base_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Init func tests executable files
set(list_of_exec_tests "")
if (USE_FUNC_TESTS)
    file(GLOB_RECURSE func_tests_source_files "${test_base_dir}/func_tests/*")
    add_executable(${exec_func_tests} ${func_tests_source_files} "${test_base_dir}/runner.cpp")
    list(APPEND list_of_exec_tests ${exec_func_tests})
endif (USE_FUNC_TESTS)

# Init perf tests executable files
if (USE_PERF_TESTS)
    file(GLOB_RECURSE perf_tests_source_files "${test_base_dir}/perf_tests/*")
    add_executable(${exec_perf_tests} ${perf_tests_source_files} "${test_base_dir}/runner.cpp")
    list(APPEND list_of_exec_tests ${exec_perf_tests})
endif (USE_PERF_TESTS)

# Create lib
foreach (dir_type "all" "mpi" "omp" "seq" "stl" "tbb")
    # Check directory existing
    set(base_dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir_type}")
    if (NOT EXISTS "${base_dir}")
        continue()
    endif ()

    # Print type of directories
    message(STATUS "-- -- ${dir_type}")

    # Create task library
    file(GLOB_RECURSE lib_source_files "${base_dir}/include/*" "${base_dir}/src/*")
    file(GLOB source_files "${base_dir}/src/*")

    list(LENGTH source_files result_length)
    set(name_lib "${TASK_NAME}_${dir_type}")
    if(result_length EQUAL 0)
        add_library(${name_lib} INTERFACE ${lib_source_files})
    else()
        add_library(${name_lib} STATIC ${lib_source_files})
    endif()
    set_target_properties(${name_lib} PROPERTIES LINKER_LANGUAGE CXX)

    # Link core library
    target_link_libraries(${exec_func_tests} PUBLIC ${name_lib} core_module_lib)
    target_link_libraries(${exec_perf_tests} PUBLIC ${name_lib} core_module_lib)
endforeach ()

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
