function run_mpi_file {
  if [ -f "${PWD}$1" ]; then mpirun -np 2 .$1; else echo "File $1 does not exist!"; fi
}

function run_omp_file {
    if [ -f "${PWD}$1" ]; then .$1; else echo "File $1 does not exist!"; fi
}

echo "##########################################"
echo "mpi"
echo "##########################################"
run_mpi_file "/build/modules/test_tasks/mpi/mpi"
echo "##########################################"
echo "omp"
echo "##########################################"
run_omp_file "/build/modules/test_tasks/omp/omp"
