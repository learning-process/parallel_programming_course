#!/bin/bash

if [[ -z "$ASAN_RUN" ]]; then
  if [[ $OSTYPE == "linux-gnu" ]]; then
    mpirun -np 4 ./build/bin/all_perf_tests --gtest_color=no
    mpirun -np 4 ./build/bin/mpi_perf_tests --gtest_color=no
  elif [[ $OSTYPE == "darwin"* ]]; then
    mpirun -np 2 ./build/bin/all_perf_tests --gtest_color=no
    mpirun -np 2 ./build/bin/mpi_perf_tests --gtest_color=no
  fi
fi
./build/bin/omp_perf_tests
./build/bin/seq_perf_tests
./build/bin/stl_perf_tests
./build/bin/tbb_perf_tests
