#include "example/threads/all/include/ops_all.hpp"

#include <mpi.h>

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include "example/common/include/common.hpp"
#include "oneapi/tbb/parallel_for.h"
#include "util/include/util.hpp"

namespace example_threads {

NesterovATestTaskALL::NesterovATestTaskALL(const InType &in) : BaseTask(in) {}

bool NesterovATestTaskALL::ValidationImpl() {
  return (GetInput() > 0) && (GetMutableOutput() == 0);
}

bool NesterovATestTaskALL::PreProcessingImpl() {
  GetMutableOutput() = 2 * GetInput();
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskALL::RunImpl() {
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
  {
    GetMutableOutput() *= num_threads;

    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      std::atomic<int> counter(0);
#pragma omp parallel default(none) shared(counter) num_threads(ppc::util::GetNumThreads())
      counter++;

      GetMutableOutput() /= counter;
    } else {
      GetMutableOutput() /= num_threads;
    }
  }

  {
    GetMutableOutput() *= num_threads;
    std::vector<std::thread> threads(num_threads);
    std::atomic<int> counter(0);
    for (std::thread &thread : threads) {
      thread = std::thread([&counter]() -> void { counter++; });
      thread.join();
    }
    GetMutableOutput() /= counter;
  }

  {
    GetMutableOutput() *= num_threads;
    std::atomic<int> counter(0);
    tbb::parallel_for(0, ppc::util::GetNumThreads(), [&](int /*i*/) -> void { counter++; });
    GetMutableOutput() /= counter;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskALL::PostProcessingImpl() {
  GetMutableOutput() -= GetInput();
  return GetMutableOutput() > 0;
}

}  // namespace example_threads
