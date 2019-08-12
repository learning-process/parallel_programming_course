// Copyright 2018 Nesterov Alexander
#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int status, rank, size;
    status = MPI_Init(&argc, &argv);
    if (status != MPI_SUCCESS) { return -1; }

    status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (status != MPI_SUCCESS) { return -1; }

    status = MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (status != MPI_SUCCESS) { return -1; }

    printf("Process: %d | Count process: %d\n", rank, size);

    status = MPI_Finalize();
    if (status != MPI_SUCCESS) { return -1; }

    return 0;
}
