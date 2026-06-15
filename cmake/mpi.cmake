include_guard()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  function(ppc_configure_openmpi)
    if(TARGET MPI::MPI_CXX)
      return()
    endif()

    set(openmpi_source_dir "${CMAKE_SOURCE_DIR}/3rdparty/openmpi")
    if(NOT EXISTS "${openmpi_source_dir}/autogen.pl")
      message(
        FATAL_ERROR
          "Open MPI source backend is missing. Initialize submodules with: git submodule update --init --recursive"
      )
    endif()

    find_program(openmpi_bash bash REQUIRED)
    find_program(openmpi_make NAMES gmake make REQUIRED)
    find_program(openmpi_perl perl REQUIRED)
    find_program(openmpi_flex flex REQUIRED)
    find_program(openmpi_autoconf autoconf REQUIRED)
    find_program(openmpi_automake automake REQUIRED)
    find_program(openmpi_libtoolize NAMES glibtoolize libtoolize REQUIRED)
    find_program(openmpi_ccache ccache)

    set(openmpi_prefix "${CMAKE_BINARY_DIR}/ppc_openmpi")
    set(openmpi_install_dir "${openmpi_prefix}/install")
    set(openmpi_build_source_dir "${openmpi_prefix}/source")
    set(openmpi_library
        "${openmpi_install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}mpi${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    set(oshmem_library
        "${openmpi_install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}oshmem${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    set(openmpi_byproducts "${openmpi_library}" "${oshmem_library}")

    set(openmpi_c_compiler "${CMAKE_C_COMPILER}")
    set(openmpi_cxx_compiler "${CMAKE_CXX_COMPILER}")
    set(openmpi_c_flags "${CMAKE_C_FLAGS} -Wno-int-conversion")
    string(STRIP "${openmpi_c_flags}" openmpi_c_flags)
    set(openmpi_ccache_env)
    if(openmpi_ccache)
      set(openmpi_c_compiler "${openmpi_ccache} ${CMAKE_C_COMPILER}")
      set(openmpi_cxx_compiler "${openmpi_ccache} ${CMAKE_CXX_COMPILER}")
      list(APPEND openmpi_ccache_env "CCACHE_BASEDIR=${CMAKE_SOURCE_DIR}"
           "CCACHE_NOHASHDIR=1")
    endif()

    set(openmpi_transport_flags
        --without-alps
        --without-lsf
        --without-ofi
        --without-pbs
        --without-portals4
        --without-psm2
        --without-sge
        --without-slurm
        --without-tm
        --with-ucx)

    set(openmpi_disabled_components
        accelerator-cuda
        accelerator-rocm
        btl-ofi
        btl-portals4
        btl-smcuda
        btl-template
        btl-uct
        btl-ugni
        btl-usnic
        coll-adapt
        coll-cuda
        coll-demo
        coll-ftagree
        coll-han
        coll-hcoll
        coll-inter
        coll-monitoring
        coll-portals4
        coll-sm
        coll-sync
        coll-ucc
        common-ofi
        fbtl-ime
        fbtl-pvfs2
        fcoll-dynamic
        fcoll-dynamic_gen2
        fcoll-vulcan
        fs-gpfs
        fs-ime
        fs-lustre
        fs-pvfs2
        hook-comm_method
        hook-demo
        memchecker-valgrind
        mpool-hugepage
        mpool-memkind
        mtl-ofi
        mtl-portals4
        mtl-psm2
        osc-monitoring
        osc-portals4
        osc-ucx
        pml-cm
        pml-monitoring
        pml-ucx
        pml-v
        rcache-gpusm
        rcache-grdma
        rcache-rgpusm
        rcache-udreg
        sharedfp-individual
        sharedfp-lockedfile
        sharedfp-sm
        smsc-cma
        smsc-knem
        smsc-xpmem
        topo-example
        topo-treematch
        vprotocol-example
        vprotocol-pessimist)
    list(JOIN openmpi_disabled_components "," openmpi_disabled_components_arg)

    set(openmpi_configure_options
        "--prefix='${openmpi_install_dir}'"
        --enable-oshmem
        --disable-io-romio
        --disable-mpi-fortran
        --disable-mpi-java
        --disable-oshmem-fortran
        --disable-oshmem-profile
        --disable-picky
        --disable-static
        --enable-mpi-ext=
        --enable-shared
        --with-hwloc=internal
        --with-libevent=internal
        --with-pmix=internal
        --with-prrte=internal
        --without-tests-examples
        "--enable-mca-no-build=${openmpi_disabled_components_arg}"
        ${openmpi_transport_flags})
    list(JOIN openmpi_configure_options " " openmpi_configure_options_arg)

    set_property(GLOBAL PROPERTY ppc_openmpi_install_dir
                                 "${openmpi_install_dir}")
    set_property(GLOBAL PROPERTY ppc_openmpi_oshmem_library "${oshmem_library}")

    file(MAKE_DIRECTORY "${openmpi_install_dir}/include"
         "${openmpi_install_dir}/lib")

    include(ProcessorCount)
    include(ExternalProject)
    ProcessorCount(openmpi_parallel_jobs)
    if(openmpi_parallel_jobs EQUAL 0)
      set(openmpi_parallel_jobs 2)
    endif()
    ExternalProject_Add(
      ppc_openmpi
      PREFIX "${openmpi_prefix}"
      SOURCE_DIR "${openmpi_build_source_dir}"
      DOWNLOAD_COMMAND "${CMAKE_COMMAND}" -E rm -rf
                       "${openmpi_build_source_dir}"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${openmpi_source_dir}"
              "${openmpi_build_source_dir}"
      CONFIGURE_COMMAND
        "${CMAKE_COMMAND}" -E env "CC=${openmpi_c_compiler}"
        "CXX=${openmpi_cxx_compiler}" "CFLAGS=${openmpi_c_flags}"
        ${openmpi_ccache_env} "${openmpi_bash}" -lc
        "cd '${openmpi_build_source_dir}' && '${openmpi_perl}' ./autogen.pl --force && ./configure ${openmpi_configure_options_arg}"
      BUILD_COMMAND
        "${CMAKE_COMMAND}" -E env ${openmpi_ccache_env} "${openmpi_make}" -C
        "${openmpi_build_source_dir}" "-j${openmpi_parallel_jobs}"
      INSTALL_COMMAND "${openmpi_make}" -C "${openmpi_build_source_dir}" install
      BUILD_BYPRODUCTS ${openmpi_byproducts} ${PPC_EXTERNAL_PROJECT_LOG_ARGS})

    set(openmpi_link_options "-Wl,-rpath,${openmpi_install_dir}/lib"
                             "-Wl,-rpath,\\$ORIGIN/../lib")

    add_library(MPI::MPI_CXX SHARED IMPORTED GLOBAL)
    add_dependencies(MPI::MPI_CXX ppc_openmpi)
    set_target_properties(
      MPI::MPI_CXX
      PROPERTIES IMPORTED_LOCATION "${openmpi_library}"
                 INTERFACE_INCLUDE_DIRECTORIES "${openmpi_install_dir}/include"
                 INTERFACE_LINK_OPTIONS "${openmpi_link_options}")

    install(DIRECTORY "${openmpi_install_dir}/" DESTINATION ".")
  endfunction()

  ppc_configure_openmpi()
else()
  find_package(MPI REQUIRED COMPONENTS C CXX)
  if(NOT MPI_FOUND)
    message(FATAL_ERROR "MPI NOT FOUND")
  endif()
endif()

function(ppc_link_mpi exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC MPI::MPI_CXX)
endfunction()
