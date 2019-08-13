// Copyright 2018 Nesterov Alexander
#include "../../../modules/test_tasks/test_mpi/ops_mpi.h"

int getMpiRank(MPI_Comm comm) {
    int out;
    MPI_Comm_rank(comm, &out);
    return out;
}

int getMpiRankPlusOne(MPI_Comm comm) {
    int out;
    MPI_Comm_rank(comm, &out);
    return (out+1);
}

int getZero(MPI_Comm comm) {
  return 0;
}

int getNonzeroMpiRank(MPI_Comm comm) {
  int out;
  MPI_Comm_rank(comm, &out);
  return (out ? out : 1);
}
