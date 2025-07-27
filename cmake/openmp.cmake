if(MSVC AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(OpenMP_C_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP C flags" FORCE)
  set(OpenMP_CXX_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP CXX flags" FORCE)
  # Pretend Clang-style OpenMP 5.0 support so CMake reports a newer version
  set(OpenMP_C_SPEC_DATE
      201811
      CACHE STRING "OpenMP C specification date" FORCE)
  set(OpenMP_CXX_SPEC_DATE
      201811
      CACHE STRING "OpenMP CXX specification date" FORCE)
endif()

function(ppc_link_threads exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC Threads::Threads)
endfunction()

function(ppc_link_openmp exec_func_lib)
  find_package(OpenMP REQUIRED)
  target_link_libraries(${exec_func_lib} PUBLIC ${OpenMP_libomp_LIBRARY}
                                                OpenMP::OpenMP_CXX)
endfunction()
