@echo off
echo "##########################################"
echo "mpi"
echo "##########################################"
dir build\modules\test_tasks\mpi
mpiexec.exe -np 4 build\modules\test_tasks\mpi\Debug\mpi.exe
echo "##########################################"
echo "omp"
echo "##########################################"
build\modules\test_tasks\omp\Debug\omp.exe
