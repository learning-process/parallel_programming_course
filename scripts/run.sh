#!/bin/bash
FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_TBB="build/bin/*_tbb"
for file in $FILES_TBB; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_MPI="build/bin/*_mpi"
for file in $FILES_MPI; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    NUM_PROC=$(cat /proc/cpuinfo|grep processor|wc -l )
    for i in {1..10}; do
        mpirun -np $NUM_PROC $file
    done
done
