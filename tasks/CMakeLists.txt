message(STATUS "Student's tasks")

list(APPEND LIST_OF_TASKS "mpi")
list(APPEND LIST_OF_TASKS "omp")
list(APPEND LIST_OF_TASKS "seq")
list(APPEND LIST_OF_TASKS "stl")
list(APPEND LIST_OF_TASKS "tbb")
list(APPEND LIST_OF_TASKS "all")

add_compile_definitions(PATH_TO_PPC_PROJECT="${CMAKE_SOURCE_DIR}")

add_library(stb_image INTERFACE)

foreach(TASK_TYPE ${LIST_OF_TASKS})
    set(PATH_TO_TASK "${CMAKE_CURRENT_SOURCE_DIR}/${TASK_TYPE}")
    get_filename_component(MODULE_NAME ${PATH_TO_TASK} NAME)
    message(STATUS      "${MODULE_NAME} tasks")
    set(exec_func_tests "${MODULE_NAME}_func_tests")
    set(exec_perf_tests "${MODULE_NAME}_perf_tests")
    set(exec_func_lib   "${MODULE_NAME}_module_lib")
    set(project_suffix  "_${MODULE_NAME}")

    SUBDIRLIST(subdirs ${PATH_TO_TASK})
    foreach(subd ${subdirs})
      if ("${subd}" MATCHES "_disabled$")
        get_filename_component(DIR_NAME ${PATH_TO_TASK} NAME)
        list(APPEND LIST_OF_REVERTS "${DIR_NAME}_${subd}")
        continue()
      endif()
      get_filename_component(PROJECT_ID ${subd} NAME)
      set(PATH_PREFIX "${PATH_TO_TASK}/${subd}")
      message(STATUS "-- ${PROJECT_ID}${project_suffix}")

      file(GLOB_RECURSE TMP_LIB_SOURCE_FILES "${PATH_PREFIX}/include/*" "${PATH_PREFIX}/src/*")
      list(APPEND LIB_SOURCE_FILES ${TMP_LIB_SOURCE_FILES})

      file(GLOB SRC_RES "${PATH_PREFIX}/src/*")
      list(APPEND SRC_RES ${TMP_SRC_RES})

      file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES "${PATH_PREFIX}/func_tests/*")
      list(APPEND FUNC_TESTS_SOURCE_FILES ${TMP_FUNC_TESTS_SOURCE_FILES})

      file(GLOB_RECURSE TMP_PERF_TESTS_SOURCE_FILES "${PATH_PREFIX}/perf_tests/*")
      list(APPEND PERF_TESTS_SOURCE_FILES ${TMP_PERF_TESTS_SOURCE_FILES})
    endforeach()

    project(${exec_func_lib})
    list(LENGTH SRC_RES RES_LEN)
    if(RES_LEN EQUAL 0)
      add_library(${exec_func_lib} INTERFACE ${LIB_SOURCE_FILES})
    else()
      add_library(${exec_func_lib} STATIC ${LIB_SOURCE_FILES})
    endif()
    set_target_properties(${exec_func_lib} PROPERTIES LINKER_LANGUAGE CXX)

    if (USE_FUNC_TESTS)
      add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES} "${PATH_TO_TASK}/runner.cpp")
      list(APPEND LIST_OF_EXEC_TESTS ${exec_func_tests})
    endif (USE_FUNC_TESTS)
    if (USE_PERF_TESTS)
      add_executable(${exec_perf_tests} ${PERF_TESTS_SOURCE_FILES} "${PATH_TO_TASK}/runner.cpp")
      list(APPEND LIST_OF_EXEC_TESTS ${exec_perf_tests})
    endif (USE_PERF_TESTS)

    foreach (EXEC_FUNC ${LIST_OF_EXEC_TESTS})
      target_link_libraries(${EXEC_FUNC} PUBLIC ${exec_func_lib} core_module_lib)
      target_link_libraries(${EXEC_FUNC} PUBLIC Threads::Threads)
      target_link_libraries(${EXEC_FUNC} PUBLIC ${OpenMP_libomp_LIBRARY})
      if( MPI_COMPILE_FLAGS )
          set_target_properties(${EXEC_FUNC} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
      endif( MPI_COMPILE_FLAGS )

      if( MPI_LINK_FLAGS )
          set_target_properties(${EXEC_FUNC} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
      endif( MPI_LINK_FLAGS )
      target_link_libraries(${EXEC_FUNC} PUBLIC ${MPI_LIBRARIES})

      add_dependencies(${EXEC_FUNC} ppc_onetbb)
      target_link_directories(${EXEC_FUNC} PUBLIC ${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
      if(NOT MSVC)
          target_link_libraries(${EXEC_FUNC} PUBLIC ${PPC_TBB_LIB_NAME})
      endif()

      target_link_directories(stb_image INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/stb)
      target_link_libraries(${EXEC_FUNC} PUBLIC stb_image)

      add_dependencies(${EXEC_FUNC} ppc_googletest)
      target_link_directories(${EXEC_FUNC} PUBLIC "${CMAKE_BINARY_DIR}/ppc_googletest/install/lib")
      target_link_libraries(${EXEC_FUNC} PUBLIC gtest gtest_main)
      enable_testing()
      add_test(NAME ${EXEC_FUNC} COMMAND ${EXEC_FUNC})

      # Install the executable
      install(TARGETS ${EXEC_FUNC} RUNTIME DESTINATION bin)
    endforeach ()

    # Install the library
    install(TARGETS ${exec_func_lib} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

    set(LIST_OF_EXEC_TESTS "")
    set(LIB_SOURCE_FILES "")
    set(SRC_RES "")
    set(FUNC_TESTS_SOURCE_FILES "")
    set(PERF_TESTS_SOURCE_FILES "")
endforeach()

set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/revert-list.txt")
file(WRITE ${OUTPUT_FILE} "${CONTENT}")
message(STATUS "revert list")
foreach (dir_name ${LIST_OF_REVERTS})
    message(STATUS "-- ${dir_name}")
    file(APPEND ${OUTPUT_FILE} "${dir_name}\n")
endforeach()
