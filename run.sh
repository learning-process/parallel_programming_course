echo "##########################################"
echo "mpi"
echo "##########################################"
mpirun -np 2 ./build/modules/test_tasks/mpi/mpi
echo "##########################################"
echo "omp"
echo "##########################################"
FILE="/build/modules/test_tasks/omp/omp"
if [ -f "${PWD}$FILE" ]; then .$FILE; else echo "File $FILE does not exist!"; fi
