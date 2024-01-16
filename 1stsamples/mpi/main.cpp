// Copyright 2023 Nesterov Alexander
#include <mpi.h>

#include <iostream>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int len_chars;
  MPI_Get_processor_name(processor_name, &len_chars);

  MPI_Barrier(MPI_COMM_WORLD);
  std::cout << "Processor = " << processor_name << std::endl;
  std::cout << "Rank = " << world_rank << std::endl;
  std::cout << "Number of processors = " << world_size << std::endl;

  MPI_Finalize();
  return 0;
}