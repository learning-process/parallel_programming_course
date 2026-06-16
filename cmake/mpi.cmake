include_guard()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_program(ppc_openmpi_c_wrapper NAMES mpicc.openmpi mpicc)
  find_program(ppc_openmpi_cxx_wrapper NAMES mpicxx.openmpi mpicxx
                                             mpic++.openmpi mpic++)
  if(ppc_openmpi_c_wrapper AND ppc_openmpi_cxx_wrapper)
    set(MPI_C_COMPILER
        "${ppc_openmpi_c_wrapper}"
        CACHE FILEPATH "MPI C compiler wrapper" FORCE)
    set(MPI_CXX_COMPILER
        "${ppc_openmpi_cxx_wrapper}"
        CACHE FILEPATH "MPI CXX compiler wrapper" FORCE)
  endif()
endif()

find_package(MPI REQUIRED COMPONENTS C CXX)
if(NOT MPI_FOUND)
  message(FATAL_ERROR "MPI NOT FOUND")
endif()

function(ppc_link_mpi exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC MPI::MPI_CXX)
endfunction()
