#!/bin/bash

if [[ -z "$ASAN_RUN" ]]; then
  if [[ $OSTYPE == "linux-gnu" ]]; then
    mpirun --oversubscribe -np 4 ./build/bin/mpi_perf_tests
  elif [[ $OSTYPE == "darwin"* ]]; then
    mpirun -np 2 ./build/bin/mpi_perf_tests
  fi
fi
./build/bin/omp_perf_tests
./build/bin/seq_perf_tests
./build/bin/stl_perf_tests
./build/bin/tbb_perf_tests
