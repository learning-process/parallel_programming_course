#!/bin/bash

check_exist_file() {
    [ -f "${PWD}$1" ]
}

echo "##########################################"
echo "mpi"
echo "##########################################"
FILE="/build/bin/test_mpi"
if check_exist_file $FILE;
then
    mpirun -np 2 .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "omp"
echo "##########################################"
FILE="/build/bin/test_omp"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "tbb"
echo "##########################################"
FILE="/build/bin/test_tbb"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi
