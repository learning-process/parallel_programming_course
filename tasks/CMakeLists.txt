project(parallel_programming_course LANGUAGES C CXX)

message(STATUS "Student's tasks")

# Test runner executables
set(FUNC_TEST_EXEC ppc_func_tests)
set(PERF_TEST_EXEC ppc_perf_tests)

# ——— Include helper scripts ——————————————————————————————————————
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

# ——— Initialize test executables —————————————————————————————————————
ppc_add_test(${FUNC_TEST_EXEC} common/runners/functional.cpp USE_FUNC_TESTS)
ppc_add_test(${PERF_TEST_EXEC} common/runners/performance.cpp USE_PERF_TESTS)

# ——— List of implementations ————————————————————————————————————————
set(PPC_IMPLEMENTATIONS "all;mpi;omp;seq;stl;tbb" CACHE STRING "Implementations to build (semicolon-separated)")

# ——— List of tasks ———————————————————————————————————————————————————
set(PPC_TASKS "all" CACHE STRING "Tasks to build (semicolon-separated, or 'all')")

# ——— Configure each subproject —————————————————————————————————————
file(
  GLOB subdirs
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/*")
set(available_tasks "")
foreach(sub IN LISTS subdirs)
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${sub}" AND NOT sub STREQUAL "common")
    list(APPEND available_tasks "${sub}")
  endif()
endforeach()
list(SORT available_tasks)

set(selected_tasks ${PPC_TASKS})
if(PPC_TASKS STREQUAL "all")
  set(selected_tasks ${available_tasks})
endif()

foreach(sub IN LISTS selected_tasks)
  if(NOT sub IN_LIST available_tasks)
    string(JOIN ", " available_tasks_message ${available_tasks})
    message(FATAL_ERROR "Unknown PPC_TASKS entry '${sub}'. Available tasks: ${available_tasks_message}")
  endif()
  ppc_configure_subproject(${sub})
endforeach()
