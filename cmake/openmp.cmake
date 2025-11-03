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
  # Link the canonical imported target if available
  if (TARGET OpenMP::OpenMP_CXX)
    target_link_libraries(${exec_func_lib} PUBLIC OpenMP::OpenMP_CXX)
  endif()

  if (APPLE)
    # Homebrew libomp common paths
    find_path(LIBOMP_INCLUDE_DIR omp.h HINTS /opt/homebrew/opt/libomp/include /usr/local/opt/libomp/include)
    find_library(LIBOMP_LIBRARY omp HINTS /opt/homebrew/opt/libomp/lib /usr/local/opt/libomp/lib)
    if (LIBOMP_INCLUDE_DIR)
      target_include_directories(${exec_func_lib} PUBLIC ${LIBOMP_INCLUDE_DIR})
    endif()
    if (LIBOMP_LIBRARY)
      target_link_libraries(${exec_func_lib} PUBLIC ${LIBOMP_LIBRARY})
      # Ensure Clang generates OpenMP code
      target_compile_options(${exec_func_lib} PUBLIC -Xclang -fopenmp)
    endif()
  endif()
endfunction()
