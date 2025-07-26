#include "example_threads/all/include/ops_all.hpp"

#include <mpi.h>

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include "example_threads/common/include/common.hpp"
#include "oneapi/tbb/parallel_for.h"
#include "util/include/util.hpp"

namespace nesterov_a_test_task_threads {

NesterovATestTaskALL::NesterovATestTaskALL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskALL::ValidationImpl() {
  return (GetInput() > 0) && (GetOutput() == 0);
}

bool NesterovATestTaskALL::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskALL::RunImpl() {
  for (InType i = 0; i < GetInput(); i++) {
    for (InType j = 0; j < GetInput(); j++) {
      for (InType k = 0; k < GetInput(); k++) {
        std::vector<InType> tmp(i + j + k, 1);
        GetOutput() += std::accumulate(tmp.begin(), tmp.end(), 0);
        GetOutput() -= i + j + k;
      }
    }
  }

  const int num_threads = ppc::util::GetNumThreads();
  {
    GetOutput() *= num_threads;

    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      std::atomic<int> counter(0);
#pragma omp parallel default(none) shared(counter) num_threads(ppc::util::GetNumThreads())
      counter++;

      GetOutput() /= counter;
    } else {
      GetOutput() /= num_threads;
    }
  }

  {
    GetOutput() *= num_threads;
    std::vector<std::thread> threads(num_threads);
    std::atomic<int> counter(0);
    for (int i = 0; i < num_threads; i++) {
      threads[i] = std::thread([&]() { counter++; });
      threads[i].join();
    }
    GetOutput() /= counter;
  }

  {
    GetOutput() *= num_threads;
    std::atomic<int> counter(0);
    tbb::parallel_for(0, ppc::util::GetNumThreads(), [&](int /*i*/) { counter++; });
    GetOutput() /= counter;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return GetOutput() > 0;
}

bool NesterovATestTaskALL::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace nesterov_a_test_task_threads
