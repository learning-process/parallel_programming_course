message(STATUS "Core components")
set(exec_func_tests "core_func_tests")
set(exec_func_lib "core_module_lib")

subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subd ${subdirs})
  get_filename_component(PROJECT_ID ${subd} NAME)
  set(PATH_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${subd}")
  set(PROJECT_ID "${PROJECT_ID}")
  message(STATUS "-- " ${PROJECT_ID})

  file(GLOB_RECURSE TMP_LIB_SOURCE_FILES ${PATH_PREFIX}/include/*
       ${PATH_PREFIX}/src/*)
  list(APPEND LIB_SOURCE_FILES ${TMP_LIB_SOURCE_FILES})

  file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES ${PATH_PREFIX}/tests/*)
  list(APPEND FUNC_TESTS_SOURCE_FILES ${TMP_FUNC_TESTS_SOURCE_FILES})
endforeach()

project(${exec_func_lib})
add_library(${exec_func_lib} STATIC ${LIB_SOURCE_FILES})
set_target_properties(${exec_func_lib} PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories to target
target_include_directories(
  ${exec_func_lib} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty
                          ${CMAKE_SOURCE_DIR}/modules ${CMAKE_SOURCE_DIR}/tasks)

foreach(
  link
  envpp
  json
  gtest
  threads
  openmp
  tbb
  mpi
  stb)
  cmake_language(CALL "ppc_link_${link}" ${exec_func_lib})
endforeach()

add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})

target_link_libraries(${exec_func_tests} PUBLIC ${exec_func_lib})

enable_testing()
add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})

# Installation rules
install(
  TARGETS ${exec_func_lib}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(TARGETS ${exec_func_tests} RUNTIME DESTINATION bin)
