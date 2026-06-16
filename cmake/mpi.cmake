include_guard()

set(PPC_MPI_EXTENSIONS_HOME
    ""
    CACHE PATH "Path to an unpacked mpi-extensions Open MPI package")

function(_ppc_json_escape output value)
  set(escaped "${value}")
  string(REPLACE "\\" "\\\\" escaped "${escaped}")
  string(REPLACE "\"" "\\\"" escaped "${escaped}")
  string(REPLACE "\n" "\\n" escaped "${escaped}")
  string(REPLACE "\r" "\\r" escaped "${escaped}")
  string(REPLACE "\t" "\\t" escaped "${escaped}")
  set(${output}
      "${escaped}"
      PARENT_SCOPE)
endfunction()

if(NOT WIN32)
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
  _ppc_json_escape(_PPC_MPI_EXTENSIONS_HOME_JSON "${PPC_MPI_EXTENSIONS_HOME}")
  _ppc_json_escape(_MPIEXEC_EXECUTABLE_JSON "${MPIEXEC_EXECUTABLE}")
  _ppc_json_escape(_PPC_MPI_EXTENSIONS_BIN_JSON "${_PPC_MPI_EXTENSIONS_BIN}")
  _ppc_json_escape(_PPC_MPI_EXTENSIONS_LIB_JSON "${_PPC_MPI_EXTENSIONS_LIB}")
  file(
    WRITE "${_PPC_MPI_RUNTIME_CONFIG}"
    "{\n"
    "  \"mpi_extensions_home\": \"${_PPC_MPI_EXTENSIONS_HOME_JSON}\",\n"
    "  \"mpi_exec\": \"${_MPIEXEC_EXECUTABLE_JSON}\",\n"
    "  \"path_prepend\": \"${_PPC_MPI_EXTENSIONS_BIN_JSON}\",\n"
    "  \"library_path_prepend\": \"${_PPC_MPI_EXTENSIONS_LIB_JSON}\",\n"
    "  \"env\": {\n"
    "    \"MPI_EXTENSIONS_HOME\": \"${_PPC_MPI_EXTENSIONS_HOME_JSON}\",\n"
    "    \"MPI_HOME\": \"${_PPC_MPI_EXTENSIONS_HOME_JSON}\",\n"
    "    \"OPAL_PREFIX\": \"${_PPC_MPI_EXTENSIONS_HOME_JSON}\",\n"
    "    \"OMPI_MCA_shmem\": \"mmap\"\n"
    "  }\n"
    "}\n")
  install(FILES "${_PPC_MPI_RUNTIME_CONFIG}" DESTINATION ".")
endif()

find_package(MPI REQUIRED COMPONENTS CXX)
if(NOT MPI_FOUND)
  message(FATAL_ERROR "MPI NOT FOUND")
endif()

function(ppc_link_mpi exec_func_lib)
  target_link_libraries(${exec_func_lib} PUBLIC MPI::MPI_CXX)
endfunction()
