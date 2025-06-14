#include <mpi.h>

#include "core/util/include/perf_test_util.hpp"

double ppc::util::GetTimeMPI() { return MPI_Wtime(); }

int ppc::util::GetMPIRank() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  return rank;
}
