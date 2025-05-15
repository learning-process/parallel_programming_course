#include "mpi/example/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstddef>
#include <vector>

bool nesterov_a_test_task_mpi::TestTaskMPI::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_mpi::TestTaskMPI::PreProcessingImpl() {
  // Init value for input and output
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_mpi::TestTaskMPI::RunImpl() {
  MultiplyMatrixBasedOnRank();
  return true;
}

void nesterov_a_test_task_mpi::TestTaskMPI::MultiplyMatrixBasedOnRank() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MultiplyRowMajor(input_, output_, rc_size_);
  } else {
    MultiplyColumnMajor(input_, output_, rc_size_);
  }
  MPI_Barrier(MPI_COMM_WORLD);
}

void nesterov_a_test_task_mpi::MultiplyRowMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size) {
  for (int i = 0; i < rc_size; ++i) {
    for (int j = 0; j < rc_size; ++j) {
      for (int k = 0; k < rc_size; ++k) {
        out[(i * rc_size) + j] += in[(i * rc_size) + k] * in[(k * rc_size) + j];
      }
    }
  }
}

void nesterov_a_test_task_mpi::MultiplyColumnMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size) {
  for (int j = 0; j < rc_size; ++j) {
    for (int k = 0; k < rc_size; ++k) {
      for (int i = 0; i < rc_size; ++i) {
        out[(i * rc_size) + j] += in[(i * rc_size) + k] * in[(k * rc_size) + j];
      }
    }
  }
}

bool nesterov_a_test_task_mpi::TestTaskMPI::PostProcessingImpl() { return true; }

std::vector<int> nesterov_a_test_task_mpi::TestTaskMPI::Get() { return output_; }
