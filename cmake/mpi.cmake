find_package(MPI REQUIRED)
if(NOT MPI_FOUND)
  message(FATAL_ERROR "MPI NOT FOUND")
endif()

function(ppc_link_mpi exec_func_lib)
  find_package(MPI REQUIRED)
  if(MPI_COMPILE_FLAGS)
    set_target_properties(${exec_func_lib} PROPERTIES COMPILE_FLAGS
                                                      "${MPI_COMPILE_FLAGS}")
  endif(MPI_COMPILE_FLAGS)

  if(MPI_LINK_FLAGS)
    set_target_properties(${exec_func_lib} PROPERTIES LINK_FLAGS
                                                      "${MPI_LINK_FLAGS}")
  endif(MPI_LINK_FLAGS)
  target_include_directories(${exec_func_lib} PUBLIC ${MPI_INCLUDE_PATH})
  target_link_libraries(${exec_func_lib} PUBLIC ${MPI_LIBRARIES})
endfunction()
