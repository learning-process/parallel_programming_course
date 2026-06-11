include_guard()

ppc_external_project_add(
  ppc_benchmark
  USE_PROJECT_CXX_STANDARD
  SOURCE_DIR
  "${CMAKE_SOURCE_DIR}/3rdparty/benchmark"
  EXCLUDE_FROM_ALL
  TRUE
  CMAKE_ARGS
  -DCMAKE_C_FLAGS=-w
  -DCMAKE_CXX_FLAGS=-w
  -DBENCHMARK_ENABLE_TESTING=OFF
  -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
  -DBENCHMARK_ENABLE_WERROR=OFF
  -DBENCHMARK_ENABLE_INSTALL=ON
  -DBENCHMARK_ENABLE_LIBPFM=OFF)

function(ppc_include_benchmark target_name)
  target_include_directories(
    ${target_name} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/benchmark/include)
  target_compile_definitions(${target_name} PUBLIC BENCHMARK_STATIC_DEFINE)
endfunction()

function(ppc_link_benchmark target_name)
  ppc_include_benchmark(${target_name})
  add_dependencies(${target_name} ppc_benchmark)
  target_link_directories(${target_name} PUBLIC
                          "${CMAKE_BINARY_DIR}/ppc_benchmark/install/lib")
  target_link_libraries(${target_name} PUBLIC benchmark Threads::Threads)
  if(WIN32)
    target_link_libraries(${target_name} PUBLIC shlwapi)
  endif()
endfunction()
