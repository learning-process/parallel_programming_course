include_guard()

set(PPC_MPI_EXTENSIONS_HOME
    ""
    CACHE PATH "Path to an unpacked mpi-extensions Open MPI package")

if(NOT WIN32)
  find_package(Python REQUIRED COMPONENTS Interpreter)

  if(NOT PPC_MPI_EXTENSIONS_HOME)
    message(
      FATAL_ERROR
        "PPC_MPI_EXTENSIONS_HOME is required on Linux and macOS. "
        "Install the mpi-extensions nightly Open MPI package and configure with "
        "-DPPC_MPI_EXTENSIONS_HOME=/path/to/mpi-extensions-openmpi")
  endif()

  get_filename_component(PPC_MPI_EXTENSIONS_HOME "${PPC_MPI_EXTENSIONS_HOME}"
                         REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
  set(PPC_MPI_EXTENSIONS_HOME
      "${PPC_MPI_EXTENSIONS_HOME}"
      CACHE PATH "Path to an unpacked mpi-extensions Open MPI package" FORCE)

  set(_PPC_MPI_EXTENSIONS_BIN "${PPC_MPI_EXTENSIONS_HOME}/bin")
  set(_PPC_MPI_EXTENSIONS_LIB "${PPC_MPI_EXTENSIONS_HOME}/lib")

  foreach(tool mpicc mpicxx mpirun mpiexec)
    if(NOT EXISTS "${_PPC_MPI_EXTENSIONS_BIN}/${tool}")
      message(
        FATAL_ERROR
          "PPC_MPI_EXTENSIONS_HOME does not contain required tool '${tool}': "
          "${_PPC_MPI_EXTENSIONS_BIN}/${tool}")
    endif()
  endforeach()

  if(NOT IS_DIRECTORY "${_PPC_MPI_EXTENSIONS_LIB}")
    message(
      FATAL_ERROR "PPC_MPI_EXTENSIONS_HOME does not contain a lib directory: "
                  "${_PPC_MPI_EXTENSIONS_LIB}")
  endif()

  set(MPI_C_COMPILER
      "${_PPC_MPI_EXTENSIONS_BIN}/mpicc"
      CACHE FILEPATH "mpi-extensions C MPI wrapper" FORCE)
  set(MPI_CXX_COMPILER
      "${_PPC_MPI_EXTENSIONS_BIN}/mpicxx"
      CACHE FILEPATH "mpi-extensions CXX MPI wrapper" FORCE)
  set(MPIEXEC_EXECUTABLE
      "${_PPC_MPI_EXTENSIONS_BIN}/mpirun"
      CACHE FILEPATH "mpi-extensions MPI launcher" FORCE)
  set(MPIEXEC_NUMPROC_FLAG
      "-np"
      CACHE STRING "MPI process count flag" FORCE)

  list(PREPEND CMAKE_PREFIX_PATH "${PPC_MPI_EXTENSIONS_HOME}")
  list(PREPEND CMAKE_BUILD_RPATH "${_PPC_MPI_EXTENSIONS_LIB}")
  list(PREPEND CMAKE_INSTALL_RPATH "${_PPC_MPI_EXTENSIONS_LIB}")

  set(_PPC_MPI_RUNTIME_CONFIG "${CMAKE_BINARY_DIR}/ppc_mpi_runtime_env.json")
  execute_process(
    COMMAND
      "${Python_EXECUTABLE}"
      "${CMAKE_SOURCE_DIR}/scripts/write_mpi_runtime_env.py" --output
      "${_PPC_MPI_RUNTIME_CONFIG}" --mpi-extensions-home
      "${PPC_MPI_EXTENSIONS_HOME}" --mpi-exec "${MPIEXEC_EXECUTABLE}"
      --path-prepend "${_PPC_MPI_EXTENSIONS_BIN}" --library-path-prepend
      "${_PPC_MPI_EXTENSIONS_LIB}"
    RESULT_VARIABLE _PPC_MPI_RUNTIME_CONFIG_RESULT
    OUTPUT_VARIABLE _PPC_MPI_RUNTIME_CONFIG_OUTPUT
    ERROR_VARIABLE _PPC_MPI_RUNTIME_CONFIG_ERROR)
  if(NOT _PPC_MPI_RUNTIME_CONFIG_RESULT EQUAL 0)
    message(
      FATAL_ERROR
        "Failed to write MPI runtime config:\n"
        "${_PPC_MPI_RUNTIME_CONFIG_OUTPUT}${_PPC_MPI_RUNTIME_CONFIG_ERROR}")
  endif()
  install(FILES "${_PPC_MPI_RUNTIME_CONFIG}" DESTINATION ".")
endif()

find_package(MPI REQUIRED COMPONENTS CXX)
if(NOT MPI_FOUND)
  message(FATAL_ERROR "MPI NOT FOUND")
endif()

function(ppc_link_mpi exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC MPI::MPI_CXX)
endfunction()
