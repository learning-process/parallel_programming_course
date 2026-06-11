include_guard()

ppc_external_project_add(
  ppc_libenvpp SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/libenvpp" CMAKE_ARGS
  -DLIBENVPP_TESTS=OFF -DLIBENVPP_EXAMPLES=OFF)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if(cmake_build_type_lower STREQUAL "debug")
  set(PPC_FMT_LIB_NAME fmtd)
else()
  set(PPC_FMT_LIB_NAME fmt)
endif()

if(WIN32)
  set(PPC_ENVPP_LIB_NAME libenvpp)
else()
  set(PPC_ENVPP_LIB_NAME envpp)
endif()

function(ppc_link_envpp exec_func_lib)
  # Add external project include directories
  target_include_directories(
    ${exec_func_lib} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/include)
  target_include_directories(
    ${exec_func_lib} SYSTEM
    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/libenvpp/external/fmt/include)

  add_dependencies(${exec_func_lib} ppc_libenvpp)
  target_link_directories(${exec_func_lib} PUBLIC
                          "${CMAKE_BINARY_DIR}/ppc_libenvpp/install/lib")
  target_link_directories(${exec_func_lib} PUBLIC
                          "${CMAKE_BINARY_DIR}/ppc_libenvpp/build")
  target_link_libraries(${exec_func_lib} PUBLIC ${PPC_ENVPP_LIB_NAME})
  target_link_libraries(${exec_func_lib} PUBLIC $<$<CONFIG:Debug>:fmtd>
                                                $<$<NOT:$<CONFIG:Debug>>:fmt>)
endfunction()
