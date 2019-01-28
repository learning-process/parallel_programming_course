echo "##########################################"
echo "mpi"
echo "##########################################"
mpirun -np 2 ./build/modules/test_tasks/mpi/mpi

echo "##########################################"
echo "omp"
echo "##########################################"
export OMP_NUM_THREADS=4
./build/modules/test_tasks/omp/omp
