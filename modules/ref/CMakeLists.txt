get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS      "${MODULE_NAME} tasks")
set(exec_func_tests "${MODULE_NAME}_func_tests")
set(exec_func_lib   "${MODULE_NAME}_module_lib")
set(project_suffix  "_${MODULE_NAME}")

SUBDIRLIST(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subd ${subdirs})
  get_filename_component(PROJECT_ID ${subd} NAME)
  set(PATH_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${subd}")
  set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
  message(STATUS "-- " ${PROJECT_ID})

  file(GLOB_RECURSE TMP_LIB_SOURCE_FILES ${PATH_PREFIX}/include/* ${PATH_PREFIX}/src/*)
  list(APPEND LIB_SOURCE_FILES ${TMP_LIB_SOURCE_FILES})

  file(GLOB TMP_SRC_RES ${PATH_PREFIX}/src/*)
  list(APPEND SRC_RES ${TMP_SRC_RES})

  file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES ${PATH_PREFIX}/func_tests/*)
  list(APPEND FUNC_TESTS_SOURCE_FILES ${TMP_FUNC_TESTS_SOURCE_FILES})
endforeach()

project(${exec_func_lib})
list(LENGTH SRC_RES RES_LEN)
if(RES_LEN EQUAL 0)
  add_library(${exec_func_lib} INTERFACE ${LIB_SOURCE_FILES})
else()
  add_library(${exec_func_lib} STATIC ${LIB_SOURCE_FILES})
endif()
set_target_properties(${exec_func_lib} PROPERTIES LINKER_LANGUAGE CXX)

add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})
target_link_libraries(${exec_func_tests} PUBLIC core_module_lib)

add_dependencies(${exec_func_tests} ppc_googletest)
target_link_directories(${exec_func_tests} PUBLIC ${CMAKE_BINARY_DIR}/ppc_googletest/install/lib)
target_link_libraries(${exec_func_tests} PUBLIC gtest gtest_main)

target_link_libraries(${exec_func_tests} PUBLIC ${exec_func_lib})

enable_testing()
add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})

CPPCHECK_TEST("${exec_func_tests}" "${FUNC_TESTS_SOURCE_FILES}")
