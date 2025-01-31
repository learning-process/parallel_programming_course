#!/bin/bash
if [[ -f build/ppc_opencv/install/bin/setup_vars_opencv4.sh ]]; then
  INSTALL_BIN_DIR="build/bin"
  source build/ppc_opencv/install/bin/setup_vars_opencv4.sh
else
  INSTALL_BIN_DIR="install/bin"
  source $INSTALL_BIN_DIR/setup_vars_opencv4.sh
fi

if [[ -z "$ASAN_RUN" ]]; then
  if [[ $OSTYPE == "linux-gnu" ]]; then
    mpirun -np 4 $INSTALL_BIN_DIR/all_perf_tests --gtest_color=no
    mpirun -np 4 $INSTALL_BIN_DIR/mpi_perf_tests --gtest_color=no
  elif [[ $OSTYPE == "darwin"* ]]; then
    mpirun -np 2 $INSTALL_BIN_DIR/all_perf_tests --gtest_color=no
    mpirun -np 2 $INSTALL_BIN_DIR/mpi_perf_tests --gtest_color=no
  fi
fi
$INSTALL_BIN_DIR/omp_perf_tests
$INSTALL_BIN_DIR/seq_perf_tests
$INSTALL_BIN_DIR/stl_perf_tests
$INSTALL_BIN_DIR/tbb_perf_tests
