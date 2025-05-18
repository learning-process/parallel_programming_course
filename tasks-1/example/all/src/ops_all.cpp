#include "example/all/include/ops_all.hpp"

#include <mpi.h>

#include <cmath>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>

#include "core/util/include/util.hpp"
#include "oneapi/tbb/parallel_for.h"

void nesterov_a_test_task_all::MatMul(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec) {
  for (int i = 0; i < rc_size; ++i) {
    for (int j = 0; j < rc_size; ++j) {
      out_vec[(i * rc_size) + j] = 0;
      for (int k = 0; k < rc_size; ++k) {
        out_vec[(i * rc_size) + j] += in_vec[(i * rc_size) + k] * in_vec[(k * rc_size) + j];
      }
    }
  }
}

void nesterov_a_test_task_all::MatMulTBB(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec) {
  tbb::parallel_for(0, ppc::util::GetPPCNumThreads(), [&](int i) { MatMul(in_vec, rc_size - i, out_vec); });
  MatMul(in_vec, rc_size, out_vec);
}

bool nesterov_a_test_task_all::TestTaskALL::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_all::TestTaskALL::PreProcessingImpl() {
  // Init value for input and output
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_all::TestTaskALL::RunImpl() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
#pragma omp parallel default(none)
    {
#pragma omp critical
      MatMul(input_, rc_size_, output_);
    }
  } else {
    MatMulTBB(input_, rc_size_, output_);
  }

  const int num_threads = ppc::util::GetPPCNumThreads();
  std::vector<std::thread> threads(num_threads);
  for (int i = 0; i < num_threads; i++) {
    threads[i] = std::thread(MatMul, std::cref(input_), rc_size_, std::ref(output_));
    threads[i].join();
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool nesterov_a_test_task_all::TestTaskALL::PostProcessingImpl() { return true; }

std::vector<int> nesterov_a_test_task_all::TestTaskALL::Get() { return output_; }
