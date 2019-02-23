#!/bin/bash
FILES_MPI="build/bin/*_mpi"
for file in $FILES_MPI; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    mpirun -np 2 $file
done

FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file
done

FILES_TBB="build/bin/*_tbb"
for file in $FILES_TBB; do
    echo "--------------------------------"
    echo $(basename $file)
    echo "--------------------------------"
    ./$file
done
