include_guard()

ppc_external_project_add(
  ppc_json USE_PROJECT_CXX_STANDARD SOURCE_DIR
  "${CMAKE_SOURCE_DIR}/3rdparty/json" CMAKE_ARGS -DJSON_BuildTests=OFF)

function(ppc_link_json exec_func_lib)
  # Add external project include directories
  target_include_directories(${exec_func_lib}
                             PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/json/include)

  add_dependencies(${exec_func_lib} ppc_json)
  target_include_directories(
    ${exec_func_lib} INTERFACE "${CMAKE_BINARY_DIR}/ppc_json/install/include")
endfunction()
