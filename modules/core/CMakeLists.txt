message(STATUS "Core module")
set(exec_func_tests "core_func_tests")
set(project_suffix "_core")

SUBDIRLIST(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subd ${subdirs})
  get_filename_component(PROJECT_ID ${subd} NAME)
  set(PATH_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${subd}")
  set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
  set(PACK_LIB "${PROJECT_ID}_lib")
  message(STATUS "-- " ${PROJECT_ID})

  project(${PACK_LIB})
  file(GLOB_RECURSE LIB_SOURCE_FILES ${PATH_PREFIX}/include/* ${PATH_PREFIX}/src/*)
  add_library(${PACK_LIB} STATIC ${LIB_SOURCE_FILES})
  set_target_properties(${PACK_LIB} PROPERTIES LINKER_LANGUAGE CXX)

  file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES ${PATH_PREFIX}/func_tests/*)
  set(FUNC_TESTS_SOURCE_FILES "${FUNC_TESTS_SOURCE_FILES};${TMP_FUNC_TESTS_SOURCE_FILES}")
endforeach()

add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})
add_dependencies(${exec_func_tests} ppc_googletest)
target_link_directories(${exec_func_tests} PUBLIC ${CMAKE_BINARY_DIR}/ppc_googletest/install/lib)
target_link_libraries(${exec_func_tests} PUBLIC gtest gtest_main)

foreach(subd ${subdirs})
  get_filename_component(PROJECT_ID ${subd} NAME)
  set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
  set(PACK_LIB "${PROJECT_ID}_lib")

  target_link_libraries(${exec_func_tests} PUBLIC ${PACK_LIB})
endforeach()

enable_testing()
add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})

CPPCHECK_TEST("${exec_func_tests}" "${FUNC_TESTS_SOURCE_FILES}")
