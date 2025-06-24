find_package(OpenMP)
if(OpenMP_FOUND)
  include_directories(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})

  if(MSVC)
    string(REPLACE "/openmp" "/openmp:llvm" OpenMP_C_FLAGS "${OpenMP_C_FLAGS}")
    string(REPLACE "/openmp" "/openmp:llvm" OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS}")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else(OpenMP_FOUND)
  message(FATAL_ERROR "OpenMP NOT FOUND")
endif(OpenMP_FOUND)
