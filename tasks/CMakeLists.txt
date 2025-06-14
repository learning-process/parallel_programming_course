message(STATUS "Student's tasks")

project("parallel_programming_course")
set(exec_func_tests "ppc_func_tests")
set(exec_perf_tests "ppc_perf_tests")

# Init func tests executable files
set(list_of_exec_tests "")
if (USE_FUNC_TESTS)
    add_executable(${exec_func_tests} "${CMAKE_CURRENT_SOURCE_DIR}/common/runners/functional.cpp")
    list(APPEND list_of_exec_tests ${exec_func_tests})
endif (USE_FUNC_TESTS)

# Init perf tests executable files
if (USE_PERF_TESTS)
    add_executable(${exec_perf_tests} "${CMAKE_CURRENT_SOURCE_DIR}/common/runners/performance.cpp")
    list(APPEND list_of_exec_tests ${exec_perf_tests})
endif (USE_PERF_TESTS)

SUBDIRLIST(subdirs ${CMAKE_CURRENT_LIST_DIR})
foreach(subd ${subdirs})
    if (subd STREQUAL "common")
        continue()
    endif ()

    set(SETTINGS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${subd}/settings.json")
    add_compile_definitions(PPC_SETTINGS_${subd}=\"${SETTINGS_PATH}\")
    add_compile_definitions(PPC_ID_${subd}=\"${subd}\")

    add_subdirectory(${subd})
endforeach()

foreach (exec_func ${list_of_exec_tests})
    enable_testing()
    add_test(NAME ${exec_func} COMMAND ${exec_func})

    # Install the executable
    install(TARGETS ${exec_func} RUNTIME DESTINATION bin)
endforeach ()

# Install the library
install(TARGETS ${name_lib} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

add_compile_definitions(PATH_TO_PPC_PROJECT="${CMAKE_SOURCE_DIR}")
