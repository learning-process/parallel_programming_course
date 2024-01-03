message(STATUS "MPI tasks")
if( USE_MPI )
  set(exec_func_tests "mpi_func_tests")
  set(exec_perf_tests "mpi_perf_tests")
  set(project_suffix "_mpi")

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

  if( MPI_COMPILE_FLAGS )
    set_target_properties(${exec_func_tests} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  endif( MPI_COMPILE_FLAGS )

  if( MPI_LINK_FLAGS )
    set_target_properties(${exec_func_tests} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
  endif( MPI_LINK_FLAGS )
  target_link_libraries(${exec_func_tests} ${MPI_LIBRARIES})
  target_link_libraries(${exec_func_tests} gtest gtest_main boost_mpi)

  #  if( MPI_COMPILE_FLAGS )
  #    set_target_properties( ${exec_perf_tests} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}" )
  #  endif( MPI_COMPILE_FLAGS )
  #
  #  if( MPI_LINK_FLAGS )
  #    set_target_properties( ${exec_perf_tests} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}" )
  #  endif( MPI_LINK_FLAGS )
  #  target_link_libraries( ${exec_perf_tests} ${MPI_LIBRARIES} )
  #  target_link_libraries(${exec_perf_tests} gtest gtest_main boost_mpi)

  enable_testing()
  add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})
  #add_test(NAME ${exec_perf_tests} COMMAND ${exec_perf_tests})

  CPPCHECK_AND_COUNTS_TESTS("${exec_func_tests}" "${FUNC_TESTS_SOURCE_FILES}")
  #CPPCHECK_AND_COUNTS_TESTS("${exec_perf_tests}" "${PERF_TESTS_SOURCE_FILES}")
else( USE_MPI )
  message(WARNING "MPI NOT BUILD!")
endif( USE_MPI )