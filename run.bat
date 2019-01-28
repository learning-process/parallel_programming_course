@echo off
echo "##########################################"
echo "mpi"
echo "##########################################"
mpiexec.exe -np 4 build\modules\test_tasks\mpi\%configuration%\mpi.exe
echo "##########################################"
echo "omp"
echo "##########################################"
build\modules\test_tasks\omp\%configuration%\omp.exe
