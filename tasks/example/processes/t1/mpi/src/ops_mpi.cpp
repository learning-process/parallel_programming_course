#include "example/processes/t1/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "example/common/include/common.hpp"
#include "util/include/util.hpp"

namespace example_processes_t1 {

NesterovATestTaskMPI::NesterovATestTaskMPI(const InType &in) : BaseTask(in) {}

bool NesterovATestTaskMPI::ValidationImpl() {
  return (GetInput() > 0) && (GetMutableOutput() == 0);
}

bool NesterovATestTaskMPI::PreProcessingImpl() {
  GetMutableOutput() = 2 * GetInput();
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskMPI::RunImpl() {
  auto input = GetInput();
  if (input == 0) {
    return false;
  }

  for (InType i = 0; i < GetInput(); i++) {
    for (InType j = 0; j < GetInput(); j++) {
      for (InType k = 0; k < GetInput(); k++) {
        std::vector<InType> tmp(i + j + k, 1);
        GetMutableOutput() += std::accumulate(tmp.begin(), tmp.end(), 0);
        GetMutableOutput() -= i + j + k;
      }
    }
  }

  const int num_threads = ppc::util::GetNumThreads();
  GetMutableOutput() *= num_threads;

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    GetMutableOutput() /= num_threads;
  } else {
    int counter = 0;
    for (int i = 0; i < num_threads; i++) {
      counter++;
    }

    if (counter != 0) {
      GetMutableOutput() /= counter;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskMPI::PostProcessingImpl() {
  GetMutableOutput() -= GetInput();
  return GetMutableOutput() > 0;
}

}  // namespace example_processes_t1
