#!/bin/bash
source build/ppc_opencv/install/bin/setup_vars_opencv4.sh

if [[ $OSTYPE == "linux-gnu" && -z "$ASAN_RUN" ]]; then
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all core_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ref_func_tests

#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all omp_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all seq_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all stl_func_tests
#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all tbb_func_tests
fi

core_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
ref_func_tests  --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating

omp_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
seq_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
stl_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
tbb_func_tests --gtest_also_run_disabled_tests --gtest_repeat=3 --gtest_recreate_environments_when_repeating
