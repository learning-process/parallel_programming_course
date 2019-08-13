// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
#define MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_

#include <mpi.h>

int getMpiRank(MPI_Comm comm);
int getMpiRankPlusOne(MPI_Comm comm);
int getZero(MPI_Comm comm);
int getNonzeroMpiRank(MPI_Comm comm);

#endif  // MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
