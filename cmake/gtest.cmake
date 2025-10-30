include(ExternalProject)

ExternalProject_Add(
  ppc_googletest
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/googletest"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest"
  BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build"
  INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install"
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
             -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
             -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
             -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DCMAKE_C_FLAGS=-w
             -DCMAKE_CXX_FLAGS=-w
             -DBUILD_GMOCK=OFF
             $<$<BOOL:MSVC>:-Dgtest_force_shared_crt=ON>
  BUILD_COMMAND
    "${CMAKE_COMMAND}" --build
    "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --config $<CONFIG>
    --parallel
  INSTALL_COMMAND
    "${CMAKE_COMMAND}" --install
    "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/build" --config $<CONFIG>
    --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_googletest/install")

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
