echo "##########################################"
echo "mpi"
echo "##########################################"
mpirun -np 2 ./build/modules/test_tasks/mpi/mpi

echo "##########################################"
echo "omp"
echo "##########################################"
./build/modules/test_tasks/omp/omp
