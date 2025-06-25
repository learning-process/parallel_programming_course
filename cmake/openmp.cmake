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

find_package(OpenMP)
if(OpenMP_FOUND)
  include_directories(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else(OpenMP_FOUND)
  message(FATAL_ERROR "OpenMP NOT FOUND")
endif(OpenMP_FOUND)
