add_compile_definitions(PPC_PATH_TO_PROJECT="${CMAKE_CURRENT_SOURCE_DIR}")

macro(SUBDIRLIST result curdir)
  file(
    GLOB children
    RELATIVE ${curdir}
    ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

option(USE_FUNC_TESTS "Enable functional tests" ON)
if(USE_FUNC_TESTS)
  message(STATUS "Enable functional tests")
  add_compile_definitions(USE_FUNC_TESTS)
endif(USE_FUNC_TESTS)

option(USE_PERF_TESTS "Enable performance tests" ON)
if(USE_PERF_TESTS)
  message(STATUS "Enable performance tests")
  add_compile_definitions(USE_PERF_TESTS)
endif(USE_PERF_TESTS)
