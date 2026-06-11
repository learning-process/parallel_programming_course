include_guard()

option(ENABLE_SYSTEM_TBB "Use system TBB instead of bundled version" OFF)

if(NOT ENABLE_SYSTEM_TBB)
  if(WIN32)
    set(ppc_onetbb_TEST_COMMAND_ARGS
        TEST_COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install/bin"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  else()
    set(ppc_onetbb_TEST_COMMAND_ARGS "")
  endif()

  ppc_external_project_add(
    ppc_onetbb
    USE_PROJECT_CXX_STANDARD
    SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/3rdparty/onetbb"
    CMAKE_ARGS
    -DTBB_STRICT=OFF
    -DTBB_TEST=OFF
    ${ppc_onetbb_TEST_COMMAND_ARGS})

  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install/"
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
else()
  find_package(TBB REQUIRED)
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if(cmake_build_type_lower STREQUAL "debug")
  set(PPC_TBB_LIB_NAME tbb_debug)
else()
  set(PPC_TBB_LIB_NAME tbb)
endif()

function(ppc_link_tbb exec_func_lib)
  if(NOT ENABLE_SYSTEM_TBB)
    # Add external project include directories
    target_include_directories(
      ${exec_func_lib} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/onetbb/include)

    add_dependencies(${exec_func_lib} ppc_onetbb)
    target_link_directories(${exec_func_lib} PUBLIC
                            ${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib)
    if(NOT MSVC)
      target_link_libraries(${exec_func_lib} PUBLIC ${PPC_TBB_LIB_NAME})
    endif()
  else()
    # Use system TBB
    target_link_libraries(${exec_func_lib} PUBLIC TBB::tbb)
  endif()
endfunction()
