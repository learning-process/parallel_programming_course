#!/bin/bash

FILES_SEQ="build/bin/*_seq"
for file in $FILES_SEQ; do
    if [ "$file" = "build/bin/*_seq" ]; then continue; fi
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
    if [ "$file" = "build/bin/*_omp" ]; then continue; fi
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_TBB="build/bin/*_tbb"
for file in $FILES_TBB; do
    if [ "$file" = "build/bin/*_tbb" ]; then continue; fi
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_STD="build/bin/*_std"
for file in $FILES_STD; do
    if [ "$file" = "build/bin/*_std" ]; then continue; fi
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file --gtest_repeat=10
done

FILES_MPI="build/bin/*_mpi"
for file in $FILES_MPI; do
    if [ "$file" = "build/bin/*_mpi" ]; then continue; fi
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    if [[ $OSTYPE == "linux-gnu" ]]; then
        NUM_PROC=$(cat /proc/cpuinfo|grep processor|wc -l)
    elif [[ $OSTYPE == "darwin"* ]]; then
        NUM_PROC=$(sysctl -a | grep machdep.cpu | grep thread_count | cut -d ' ' -f 2)
    else
        echo "Unknown OS"
        NUM_PROC="1"
    fi
    echo "NUM_PROC: " $NUM_PROC
    for i in {1..10}; do
        mpirun -np $NUM_PROC $file || exit 1
    done
done
