#!/bin/bash

if [[ $OSTYPE == "linux-gnu" && -z "$ASAN_RUN" ]]; then
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/core_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/ref_func_tests

#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/all_func_tests
#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/mpi_func_tests
fi

./build/bin/core_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
./build/bin/ref_func_tests  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating

if [[ -z "$ASAN_RUN" ]]; then
  if [[ $OSTYPE == "linux-gnu" ]]; then
    mpirun --oversubscribe -np $PROC_COUNT ./build/bin/sample_mpi
    mpirun --oversubscribe -np $PROC_COUNT ./build/bin/sample_mpi_boost
    mpirun --oversubscribe -np $PROC_COUNT ./build/bin/all_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
    mpirun --oversubscribe -np $PROC_COUNT ./build/bin/mpi_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
  elif [[ $OSTYPE == "darwin"* ]]; then
    mpirun -np $PROC_COUNT ./build/bin/sample_mpi
    mpirun -np $PROC_COUNT ./build/bin/sample_mpi_boost
    mpirun -np $PROC_COUNT ./build/bin/all_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
    mpirun -np $PROC_COUNT ./build/bin/mpi_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
  fi
fi
