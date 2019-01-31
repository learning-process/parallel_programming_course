@echo off
echo "##########################################"
echo "mpi"
echo "##########################################"
mpiexec.exe -np 4 build\modules\test_tasks\mpi\Debug\mpi.exe

echo "##########################################"
echo "omp"
echo "##########################################"
build\modules\test_tasks\omp\Debug\omp.exe

echo "##########################################"
echo "tbb"
echo "##########################################"
build\modules\test_tasks\tbb\Debug\tbb.exe
