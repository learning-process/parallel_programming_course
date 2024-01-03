if ( USE_TBB )
  message(STATUS "TBB tasks")
  set(exec_func_tests "tbb_func_tests")
  set(exec_perf_tests "tbb_perf_tests")
  set(project_suffix "_tbb")

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

    #  file(GLOB_RECURSE TMP_PERF_TESTS_SOURCE_FILES ${PATH_PREFIX}/perf_tests/*)
    #  set(PERF_TESTS_SOURCE_FILES "${PERF_TESTS_SOURCE_FILES};${TMP_PERF_TESTS_SOURCE_FILES}")
  endforeach()

  if (LINUX OR APPLE)
    link_directories(${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
  endif (LINUX OR APPLE)

  add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})
  target_link_libraries(${exec_func_tests} task_core_lib)

#  add_executable( ${exec_perf_tests} ${PERF_TESTS_SOURCE_FILES} )
#  target_link_libraries(${exec_perf_tests} task_core_lib)

  foreach(subd ${subdirs})
    get_filename_component(PROJECT_ID ${subd} NAME)
    set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
    set(PACK_LIB "${PROJECT_ID}_lib")

    target_link_libraries(${exec_func_tests} ${PACK_LIB})
    #  target_link_libraries(${exec_perf_tests} ${PACK_LIB})
  endforeach()

  if (LINUX OR APPLE)
    add_dependencies(${exec_func_tests} ppc_onetbb)
  endif (LINUX OR APPLE)
  target_link_libraries(${exec_func_tests} gtest gtest_main tbb)
  #  if (APPLE)
  #    add_dependencies(${exec_perf_tests} ppc_onetbb)
  #  endif (APPLE)
  #  target_link_libraries(${exec_perf_tests} gtest gtest_main tbb)

  enable_testing()
  add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})
  #add_test(NAME ${exec_perf_tests} COMMAND ${exec_perf_tests})

  CPPCHECK_AND_COUNTS_TESTS("${exec_func_tests}" "${FUNC_TESTS_SOURCE_FILES}")
  #CPPCHECK_AND_COUNTS_TESTS("${exec_perf_tests}" "${PERF_TESTS_SOURCE_FILES}")
else ( USE_TBB )
  message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif ( USE_TBB )
