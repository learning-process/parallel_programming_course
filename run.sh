#!/bin/bash

check_exist_file() {
    [ -f "${PWD}$1" ]
}

echo "##########################################"
echo "mpi"
echo "##########################################"
FILE="/build/bin/mpi_test"
if check_exist_file $FILE;
then
    mpirun -np 2 .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "omp"
echo "##########################################"
FILE="/build/bin/omp_test"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi

echo "##########################################"
echo "tbb"
echo "##########################################"
FILE="/build/bin/tbb_test"
if check_exist_file $FILE;
then
    .$FILE;
else
    echo "File $FILE not exists!";
fi
