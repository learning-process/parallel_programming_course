include_guard()

set(osh_target OSH::OSH)

function(ppc_configure_osh)
  if(TARGET ${osh_target})
    return()
  endif()

  if(NOT ppc_osh_supported)
    return()
  endif()

  ppc_configure_openmpi()
  get_property(openmpi_install_dir GLOBAL PROPERTY ppc_openmpi_install_dir)
  get_property(oshmem_library GLOBAL PROPERTY ppc_openmpi_oshmem_library)

  add_library(${osh_target} SHARED IMPORTED GLOBAL)
  add_dependencies(${osh_target} ppc_openmpi)
  set_target_properties(
    ${osh_target}
    PROPERTIES IMPORTED_LOCATION "${oshmem_library}"
               INTERFACE_INCLUDE_DIRECTORIES "${openmpi_install_dir}/include")
  target_link_libraries(${osh_target} INTERFACE MPI::MPI_CXX)
endfunction()

function(ppc_link_osh target)
  if(NOT ppc_osh_supported)
    return()
  endif()

  ppc_configure_osh()
  if(NOT TARGET ${osh_target})
    message(FATAL_ERROR "OSH backend target was not configured.")
  endif()

  target_link_libraries(${target} PUBLIC ${osh_target})
endfunction()
