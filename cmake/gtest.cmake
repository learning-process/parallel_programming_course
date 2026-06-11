include_guard()

if(MSVC)
  set(ppc_googletest_MSVC_ARGS -Dgtest_force_shared_crt=ON)
else()
  set(ppc_googletest_MSVC_ARGS "")
endif()

ppc_external_project_add(
  ppc_googletest
  USE_PROJECT_CXX_STANDARD
  SOURCE_DIR
  "${CMAKE_SOURCE_DIR}/3rdparty/googletest"
  CMAKE_ARGS
  -DCMAKE_C_FLAGS=-w
  -DCMAKE_CXX_FLAGS=-w
  -DBUILD_GMOCK=OFF
  ${ppc_googletest_MSVC_ARGS})

function(ppc_link_gtest exec_func_lib)
  # Add external project include directories
  target_include_directories(
    ${exec_func_lib}
    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/googletest/googletest/include)

  add_dependencies(${exec_func_lib} ppc_googletest)
  target_link_directories(${exec_func_lib} PUBLIC
                          "${CMAKE_BINARY_DIR}/ppc_googletest/install/lib")
  target_link_libraries(${exec_func_lib} PUBLIC gtest gtest_main)
endfunction()
