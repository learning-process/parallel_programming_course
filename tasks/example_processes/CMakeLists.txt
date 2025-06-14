get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(CURRENT_DIR_NAME)

set(exec_func_tests "ppc_func_tests")
set(exec_perf_tests "ppc_perf_tests")
set(test_base_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Init func tests executable files
set(list_of_exec_tests "")
if (USE_FUNC_TESTS)
    file(GLOB_RECURSE func_tests_source_files "${test_base_dir}/functional/*")
    target_sources(${exec_func_tests} PRIVATE ${func_tests_source_files})
    list(APPEND list_of_exec_tests ${exec_func_tests})
endif (USE_FUNC_TESTS)

# Init perf tests executable files
if (USE_PERF_TESTS)
    file(GLOB_RECURSE perf_tests_source_files "${test_base_dir}/performance/*")
    target_sources(${exec_perf_tests} PRIVATE ${perf_tests_source_files})
    list(APPEND list_of_exec_tests ${exec_perf_tests})
endif (USE_PERF_TESTS)

# Print student task name
get_filename_component(TASK_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
message(STATUS "-- ${TASK_NAME}")

set(technologies "all" "mpi" "omp" "seq" "stl" "tbb")

# Create lib
foreach (dir_type ${technologies})
    # Check directory existing
    set(base_dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir_type}")
    if (NOT EXISTS "${base_dir}")
        continue()
    endif ()

    # Print type of directories
    message(STATUS "   -- ${dir_type}")

    # Create task library
    file(GLOB_RECURSE lib_source_files "${base_dir}/include/*.h" "${base_dir}/include/*.hpp" "${base_dir}/src/*.cpp")
    file(GLOB source_files "${base_dir}/src/*.cpp")

    list(LENGTH source_files result_length)
    set(name_lib "${TASK_NAME}_${dir_type}")
    if(result_length EQUAL 0)
        add_library(${name_lib} INTERFACE ${lib_source_files})
    else()
        add_library(${name_lib} STATIC ${lib_source_files})
    endif()
    set_target_properties(${name_lib} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${name_lib} PUBLIC core_module_lib)

    # Link core library
    foreach (exec_func ${list_of_exec_tests})
        target_link_libraries(${exec_func} PUBLIC ${name_lib})
    endforeach ()
endforeach ()
