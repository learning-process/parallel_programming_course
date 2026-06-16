include_guard()

set(osh_target OSH::OSH)

function(ppc_configure_openmpi_osh)
  if(TARGET ${osh_target})
    return()
  endif()

  set(osh_include_hints ${MPI_CXX_INCLUDE_DIRS})
  find_path(
    OSH_INCLUDE_DIR
    NAMES shmem.h
    HINTS ${osh_include_hints})

  set(osh_library_hints)
  foreach(mpi_library IN LISTS MPI_CXX_LIBRARIES)
    get_filename_component(mpi_library_dir "${mpi_library}" DIRECTORY)
    if(mpi_library_dir)
      list(APPEND osh_library_hints "${mpi_library_dir}")
    endif()
  endforeach()
  list(REMOVE_DUPLICATES osh_library_hints)

  find_library(
    OSH_LIBRARY
    NAMES oshmem
    HINTS ${osh_library_hints})

  if(NOT OSH_INCLUDE_DIR OR NOT OSH_LIBRARY)
    message(
      FATAL_ERROR
        "OSH was selected, but Open MPI OSH was not found. Install Open MPI with OSH support, for example: sudo apt install openmpi-bin libopenmpi-dev"
    )
  endif()

  add_library(${osh_target} UNKNOWN IMPORTED GLOBAL)
  set_target_properties(
    ${osh_target} PROPERTIES IMPORTED_LOCATION "${OSH_LIBRARY}"
                             INTERFACE_INCLUDE_DIRECTORIES "${OSH_INCLUDE_DIR}")
  target_link_libraries(${osh_target} INTERFACE MPI::MPI_CXX)
endfunction()

function(ppc_configure_sos_osh)
  if(TARGET ${osh_target})
    return()
  endif()

  set(sos_source_dir "${CMAKE_SOURCE_DIR}/3rdparty/SOS")
  if(NOT EXISTS "${sos_source_dir}/autogen.sh")
    message(
      FATAL_ERROR
        "SOS submodule is missing. Initialize submodules with: git submodule update --init --recursive"
    )
  endif()
  if(NOT EXISTS "${sos_source_dir}/modules/tests-sos/test/Makefile.am")
    message(
      FATAL_ERROR
        "SOS tests-sos submodule is missing. Initialize submodules with: git submodule update --init --recursive"
    )
  endif()

  find_program(sos_bash bash REQUIRED)
  find_program(sos_make NAMES gmake make REQUIRED)
  find_program(sos_pkg_config pkg-config REQUIRED)
  find_program(sos_ccache ccache)

  execute_process(
    COMMAND "${sos_pkg_config}" --variable=prefix libfabric
    RESULT_VARIABLE sos_pkg_config_result
    OUTPUT_VARIABLE sos_ofi_prefix
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT sos_pkg_config_result EQUAL 0 OR sos_ofi_prefix STREQUAL "")
    message(
      FATAL_ERROR
        "SOS OSH backend requires libfabric. Install it, for example: brew install libfabric"
    )
  endif()

  include(ProcessorCount)
  include(ExternalProject)
  ProcessorCount(sos_parallel_jobs)
  if(sos_parallel_jobs EQUAL 0)
    set(sos_parallel_jobs 2)
  endif()

  set(sos_prefix "${CMAKE_BINARY_DIR}/ppc_sos")
  set(sos_install_dir "${sos_prefix}/install")
  set(sos_build_source_dir "${sos_prefix}/source")
  set(sos_build_dir "${sos_prefix}/build")
  set(sos_library
      "${sos_install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}sma${CMAKE_SHARED_LIBRARY_SUFFIX}"
  )
  set(sos_versioned_library
      "${sos_install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}sma.0${CMAKE_SHARED_LIBRARY_SUFFIX}"
  )

  file(MAKE_DIRECTORY "${sos_install_dir}/include" "${sos_install_dir}/lib")

  set(sos_c_compiler "${MPI_C_COMPILER}")
  set(sos_cxx_compiler "${MPI_CXX_COMPILER}")
  set(sos_ccache_env)
  if(sos_ccache)
    set(sos_c_compiler "${sos_ccache} ${MPI_C_COMPILER}")
    set(sos_cxx_compiler "${sos_ccache} ${MPI_CXX_COMPILER}")
    set(sos_ccache_env
        "CCACHE_BASEDIR='${CMAKE_SOURCE_DIR}' CCACHE_NOHASHDIR=1 ")
  endif()

  set(sos_configure_args
      "--prefix='${sos_install_dir}'"
      "--with-ofi='${sos_ofi_prefix}'"
      --enable-pmi-mpi
      --disable-fortran
      --disable-static
      --enable-shared
      "CC='${sos_c_compiler}'"
      "CXX='${sos_cxx_compiler}'")
  list(JOIN sos_configure_args " " sos_configure_args_line)

  string(
    CONCAT sos_configure_command
           "cd '${sos_build_source_dir}' && ./autogen.sh"
           " && mkdir -p '${sos_build_dir}'"
           " && cd '${sos_build_dir}'"
           " && ${sos_ccache_env}'${sos_build_source_dir}/configure' "
           "${sos_configure_args_line}")
  string(
    CONCAT sos_install_command
           "${sos_ccache_env}'${sos_make}' -C '${sos_build_dir}' install"
           " && install_name_tool -id '@rpath/libsma.0.dylib' "
           "'${sos_versioned_library}'")

  ExternalProject_Add(
    ppc_sos
    PREFIX "${sos_prefix}"
    SOURCE_DIR "${sos_build_source_dir}"
    DOWNLOAD_COMMAND "${CMAKE_COMMAND}" -E rm -rf "${sos_build_source_dir}"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${sos_source_dir}"
            "${sos_build_source_dir}"
    CONFIGURE_COMMAND "${sos_bash}" -lc "${sos_configure_command}"
    BUILD_COMMAND
      "${sos_bash}" -lc
      "${sos_ccache_env}'${sos_make}' -C '${sos_build_dir}' -j${sos_parallel_jobs}"
    INSTALL_COMMAND "${sos_bash}" -lc "${sos_install_command}"
    BUILD_BYPRODUCTS "${sos_library}" "${sos_versioned_library}"
                     ${PPC_EXTERNAL_PROJECT_LOG_ARGS})

  add_library(${osh_target} UNKNOWN IMPORTED GLOBAL)
  add_dependencies(${osh_target} ppc_sos)
  set_target_properties(
    ${osh_target}
    PROPERTIES IMPORTED_LOCATION "${sos_library}"
               INTERFACE_INCLUDE_DIRECTORIES "${sos_install_dir}/include"
               INTERFACE_LINK_OPTIONS "-Wl,-rpath,@loader_path/../lib")
  target_link_libraries(${osh_target} INTERFACE MPI::MPI_CXX)

  install(
    DIRECTORY "${sos_install_dir}/"
    DESTINATION "."
    USE_SOURCE_PERMISSIONS)
endfunction()

function(ppc_configure_osh)
  if(TARGET ${osh_target})
    return()
  endif()

  if(NOT ppc_osh_supported)
    return()
  endif()

  if(ppc_osh_backend STREQUAL "openmpi")
    ppc_configure_openmpi_osh()
  elseif(ppc_osh_backend STREQUAL "sos")
    ppc_configure_sos_osh()
  else()
    message(FATAL_ERROR "Unknown OSH backend: ${ppc_osh_backend}")
  endif()
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
  if(TARGET ppc_sos)
    add_dependencies(${target} ppc_sos)
  endif()
endfunction()
