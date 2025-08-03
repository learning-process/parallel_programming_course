find_package(MPI REQUIRED COMPONENTS CXX)
if(NOT MPI_FOUND)
  message(FATAL_ERROR "MPI NOT FOUND")
endif()

function(ppc_link_mpi exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC MPI::MPI_CXX)
endfunction()
