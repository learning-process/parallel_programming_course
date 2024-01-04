message(STATUS "Sequential tasks")
if ( USE_SEQ )
  set(exec_func_tests "seq_func_tests")
  set(exec_perf_tests "seq_perf_tests")
  set(project_suffix "_seq")

  SUBDIRLIST(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

  foreach(subd ${subdirs})
    get_filename_component(PROJECT_ID ${subd} NAME)
    set(PATH_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${subd}")
    set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
    set(PACK_LIB "${PROJECT_ID}_lib")
    message(STATUS "-- " ${PROJECT_ID})

    project(${PACK_LIB})
    file(GLOB_RECURSE LIB_SOURCE_FILES ${PATH_PREFIX}/include/* ${PATH_PREFIX}/src/*)

    file(GLOB SRC_RES ${PATH_PREFIX}/src/*)
    list(LENGTH SRC_RES RES_LEN)
    if(RES_LEN EQUAL 0)
      add_library(${PACK_LIB} INTERFACE ${LIB_SOURCE_FILES})
    else()
      add_library(${PACK_LIB} STATIC ${LIB_SOURCE_FILES})
    endif()

    set_target_properties(${PACK_LIB} PROPERTIES LINKER_LANGUAGE CXX)

    file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES ${PATH_PREFIX}/func_tests/*)
    set(FUNC_TESTS_SOURCE_FILES "${FUNC_TESTS_SOURCE_FILES};${TMP_FUNC_TESTS_SOURCE_FILES}")

    #  file(GLOB_RECURSE TMP_PERF_TESTS_SOURCE_FILES ${PATH_PREFIX}/perf_tests/*)
    #  set(PERF_TESTS_SOURCE_FILES "${PERF_TESTS_SOURCE_FILES};${TMP_PERF_TESTS_SOURCE_FILES}")
  endforeach()

  add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})
  target_link_libraries(${exec_func_tests} PUBLIC task_core_lib)

  add_dependencies(${exec_func_tests} ppc_googletest)
  target_link_directories(${exec_func_tests} PUBLIC ${CMAKE_BINARY_DIR}/ppc_googletest/install/lib)
  if (NOT MSVC)
    target_link_libraries(${exec_func_tests} PUBLIC gtest gtest_main)
  endif ()

  #add_executable( ${exec_perf_tests} ${PERF_TESTS_SOURCE_FILES} )
  #target_link_libraries(${exec_perf_tests} PUBLIC task_core_lib)
  #add_dependencies(${exec_perf_tests} ppc_googletest)
  #target_link_directories(${exec_perf_tests} PUBLIC ${CMAKE_BINARY_DIR}/ppc_googletest/install/lib)
  #if (NOT MSVC)
  #  target_link_libraries(${exec_perf_tests} PUBLIC gtest gtest_main)
  #endif ()

  foreach(subd ${subdirs})
    get_filename_component(PROJECT_ID ${subd} NAME)
    set(PROJECT_ID "${PROJECT_ID}${project_suffix}")
    set(PACK_LIB "${PROJECT_ID}_lib")

    target_link_libraries(${exec_func_tests} PUBLIC ${PACK_LIB})
    #  target_link_libraries(${exec_perf_tests} PUBLIC ${PACK_LIB})
  endforeach()

  enable_testing()
  add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})
  #add_test(NAME ${exec_perf_tests} COMMAND ${exec_perf_tests})

  CPPCHECK_TEST("${exec_func_tests}" "${FUNC_TESTS_SOURCE_FILES}")
  #CPPCHECK_TEST("${exec_perf_tests}" "${PERF_TESTS_SOURCE_FILES}")
else( USE_SEQ )
  message(WARNING "SEQUENTIAL NOT BUILD!")
endif( USE_SEQ )
