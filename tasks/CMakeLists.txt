project(parallel_programming_course LANGUAGES C CXX)

message(STATUS "Student's tasks")

# ——— Testing options ————————————————————————————————————————
option(USE_FUNC_TESTS "Enable functional tests" OFF)
option(USE_PERF_TESTS "Enable performance tests" OFF)

# Test runner executables
set(FUNC_TEST_EXEC ppc_func_tests)
set(PERF_TEST_EXEC ppc_perf_tests)

# ——— Include helper scripts ——————————————————————————————————————
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

# ——— Initialize test executables —————————————————————————————————————
ppc_add_test(${FUNC_TEST_EXEC} common/runners/functional.cpp USE_FUNC_TESTS)
ppc_add_test(${PERF_TEST_EXEC} common/runners/performance.cpp USE_PERF_TESTS)

# ——— List of implementations ————————————————————————————————————————
set(IMPLEMENTATIONS all mpi omp seq stl tbb)

# ——— Configure each subproject —————————————————————————————————————
file(GLOB subdirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(sub IN LISTS subdirs)
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${sub}" AND NOT sub STREQUAL "common")
        ppc_configure_subproject(${sub})
    endif()
endforeach()
