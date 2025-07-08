message(STATUS "Core components")
set(EXEC_FUNC_TESTS "core_func_tests")
set(EXEC_FUNC_LIB "core_module_lib")

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

project(${EXEC_FUNC_LIB})
add_library(${EXEC_FUNC_LIB} STATIC ${LIB_SOURCE_FILES})
set_target_properties(${EXEC_FUNC_LIB} PROPERTIES LINKER_LANGUAGE CXX)

# Add include directories to target
target_include_directories(
  ${EXEC_FUNC_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty
                          ${CMAKE_SOURCE_DIR}/modules ${CMAKE_SOURCE_DIR}/tasks)

ppc_link_envpp(${EXEC_FUNC_LIB})
ppc_link_json(${EXEC_FUNC_LIB})
ppc_link_gtest(${EXEC_FUNC_LIB})
ppc_link_threads(${EXEC_FUNC_LIB})
ppc_link_openmp(${EXEC_FUNC_LIB})
ppc_link_tbb(${EXEC_FUNC_LIB})
ppc_link_mpi(${EXEC_FUNC_LIB})
ppc_link_stb(${EXEC_FUNC_LIB})

add_executable(${EXEC_FUNC_TESTS} ${FUNC_TESTS_SOURCE_FILES})

target_link_libraries(${EXEC_FUNC_TESTS} PUBLIC ${EXEC_FUNC_LIB})

enable_testing()
add_test(NAME ${EXEC_FUNC_TESTS} COMMAND ${EXEC_FUNC_TESTS})

# Installation rules
install(
  TARGETS ${EXEC_FUNC_LIB}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(TARGETS ${EXEC_FUNC_TESTS} RUNTIME DESTINATION bin)
