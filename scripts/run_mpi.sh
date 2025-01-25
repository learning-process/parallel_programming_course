#!/bin/bash
source build/ppc_opencv/install/bin/setup_vars_opencv4.sh

if [[ $OSTYPE == "linux-gnu" && -z "$ASAN_RUN" ]]; then
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all core_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ref_func_tests

#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all all_func_tests
#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all mpi_func_tests
fi

core_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
ref_func_tests  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating

if [[ -z "$ASAN_RUN" ]]; then
    mpirun $1 -np $PROC_COUNT all_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
    mpirun $1 -np $PROC_COUNT mpi_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
fi
