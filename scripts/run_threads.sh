#!/bin/bash
if [[ -f build/ppc_opencv/install/bin/setup_vars_opencv4.sh ]]; then
  INSTALL_BIN_DIR="build/bin"
  source build/ppc_opencv/install/bin/setup_vars_opencv4.sh
else
  INSTALL_BIN_DIR="install/bin"
  source $INSTALL_BIN_DIR/setup_vars_opencv4.sh
fi

if [[ $OSTYPE == "linux-gnu" && -z "$ASAN_RUN" ]]; then
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/core_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/ref_func_tests

#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/omp_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/seq_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/stl_func_tests
#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $INSTALL_BIN_DIR/tbb_func_tests
fi

$INSTALL_BIN_DIR/core_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
$INSTALL_BIN_DIR/ref_func_tests  --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating

$INSTALL_BIN_DIR/omp_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
$INSTALL_BIN_DIR/seq_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
$INSTALL_BIN_DIR/stl_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
$INSTALL_BIN_DIR/tbb_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
