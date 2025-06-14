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

  file(GLOB_RECURSE TMP_FUNC_TESTS_SOURCE_FILES ${PATH_PREFIX}/tests/*)
  list(APPEND FUNC_TESTS_SOURCE_FILES ${TMP_FUNC_TESTS_SOURCE_FILES})
endforeach()

project(${exec_func_lib})
add_library(${exec_func_lib} STATIC ${LIB_SOURCE_FILES})
set_target_properties(${exec_func_lib} PROPERTIES LINKER_LANGUAGE CXX)

add_dependencies(${exec_func_lib} ppc_libenvpp)
target_link_directories(${exec_func_lib} PUBLIC "${CMAKE_BINARY_DIR}/ppc_libenvpp/install/lib")
target_link_directories(${exec_func_lib} PUBLIC "${CMAKE_BINARY_DIR}/ppc_libenvpp/build")

if(MSVC)
  target_link_libraries(${exec_func_lib} PUBLIC libenvpp)
else()
  target_link_libraries(${exec_func_lib} PUBLIC envpp)
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_libraries(${exec_func_lib} PUBLIC fmtd)
else()
  target_link_libraries(${exec_func_lib} PUBLIC fmt)
endif()

add_dependencies(${exec_func_lib} ppc_json)
target_link_directories(${exec_func_lib} INTERFACE "${CMAKE_BINARY_DIR}/ppc_json/install/include")

add_dependencies(${exec_func_lib} ppc_googletest)
target_link_directories(${exec_func_lib} PUBLIC "${CMAKE_BINARY_DIR}/ppc_googletest/install/lib")
target_link_libraries(${exec_func_lib} PUBLIC gtest gtest_main)

target_link_libraries(${exec_func_lib} PUBLIC Threads::Threads)

find_package(OpenMP REQUIRED)
target_link_libraries(${exec_func_lib} PUBLIC ${OpenMP_libomp_LIBRARY} OpenMP::OpenMP_CXX)

add_dependencies(${exec_func_lib} ppc_onetbb)
target_link_directories(${exec_func_lib} PUBLIC ${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
if(NOT MSVC)
    target_link_libraries(${exec_func_lib} PUBLIC ${PPC_TBB_LIB_NAME})
endif()

find_package(MPI REQUIRED)
if( MPI_COMPILE_FLAGS )
  set_target_properties(${exec_func_lib} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif( MPI_COMPILE_FLAGS )

if( MPI_LINK_FLAGS )
  set_target_properties(${exec_func_lib} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif( MPI_LINK_FLAGS )
target_link_libraries(${exec_func_lib} PUBLIC ${MPI_LIBRARIES})


add_library(stb_image STATIC ${CMAKE_SOURCE_DIR}/3rdparty/stb_image_wrapper.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/stb)
target_link_libraries(${exec_func_lib} PUBLIC stb_image)

add_executable(${exec_func_tests} ${FUNC_TESTS_SOURCE_FILES})

target_link_libraries(${exec_func_tests} PUBLIC ${exec_func_lib})

enable_testing()
add_test(NAME ${exec_func_tests} COMMAND ${exec_func_tests})

# Installation rules
install(TARGETS ${exec_func_lib}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS ${exec_func_tests}
        RUNTIME DESTINATION bin)
