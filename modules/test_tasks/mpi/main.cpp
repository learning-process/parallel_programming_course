// Copyright 2018 Nesterov Alexander
#include <assert.h>
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    int status, rank, size;
    status = MPI_Init(&argc, &argv);
    if (status != MPI_SUCCESS) { return -1; }

    status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (status != MPI_SUCCESS) { return -1; }

    status = MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (status != MPI_SUCCESS) { return -1; }

    std::cout << "Process #" << rank << '\n';
    std::cout << "Count process: " << size << '\n';

    status = MPI_Finalize();
    if (status != MPI_SUCCESS) { return -1; }

    return 0;
}
