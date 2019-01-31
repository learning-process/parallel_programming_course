#!/bin/bash

check_exist_file() {
    [ -f "${PWD}$1" ]
}

echo "##########################################"
echo "mpi"
echo "##########################################"
FILE="/build/modules/test_tasks/mpi/mpi"
if check_exist_file $FILE;
then
    mpirun -np 2 .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "omp"
echo "##########################################"
FILE="/build/modules/test_tasks/omp/omp"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "tbb"
echo "##########################################"
FILE="/build/modules/test_tasks/tbb/tbb"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi
