include(ExternalProject)

option(ENABLE_SYSTEM_TBB "Use system TBB instead of bundled version" OFF)

if(NOT ENABLE_SYSTEM_TBB)
  if(WIN32)
    set(ppc_onetbb_TEST_COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install/bin"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  else()
    set(ppc_onetbb_TEST_COMMAND "")
  endif()

  ExternalProject_Add(
    ppc_onetbb
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/onetbb"
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build"
    INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install"
    CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
               -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
               -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
               -DCMAKE_CXX_STANDARD_REQUIRED=ON
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DTBB_STRICT=OFF
               -DTBB_TEST=OFF
    BUILD_COMMAND
      "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build"
      --config $<CONFIG> --parallel
    INSTALL_COMMAND
      "${CMAKE_COMMAND}" --install
      "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/build" --config $<CONFIG> --prefix
      "${CMAKE_CURRENT_BINARY_DIR}/ppc_onetbb/install"
    TEST_COMMAND ${ppc_onetbb_TEST_COMMAND})

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
