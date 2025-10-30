include(ExternalProject)

ExternalProject_Add(
  ppc_json
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/json"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/ppc_json"
  BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build"
  INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/install"
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
             -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
             -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
             -DCMAKE_CXX_STANDARD_REQUIRED=ON
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DJSON_BuildTests=OFF
  BUILD_COMMAND
    "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build"
    --config $<CONFIG> --parallel
  INSTALL_COMMAND
    "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/build"
    --config $<CONFIG> --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_json/install")

function(ppc_link_json exec_func_lib)
  # Add external project include directories
  target_include_directories(${exec_func_lib}
                             PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/json/include)

  add_dependencies(${exec_func_lib} ppc_json)
  target_include_directories(
    ${exec_func_lib} INTERFACE "${CMAKE_BINARY_DIR}/ppc_json/install/include")
endfunction()
