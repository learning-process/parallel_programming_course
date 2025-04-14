#include "mpi/example/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstddef>
#include <vector>

bool nesterov_a_test_task_mpi::TestTaskMPI::PreProcessingImpl() {
  // Init value for input and output
  unsigned int input_size = task_data->inputs_count[0];
  auto *in_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  input_ = std::vector<int>(in_ptr, in_ptr + input_size);

  unsigned int output_size = task_data->outputs_count[0];
  output_ = std::vector<int>(output_size, 0);

  rc_size_ = static_cast<int>(std::sqrt(input_size));
  return true;
}

bool nesterov_a_test_task_mpi::TestTaskMPI::ValidationImpl() {
  // Check equality of counts elements
  return task_data->inputs_count[0] == task_data->outputs_count[0];
}

bool nesterov_a_test_task_mpi::TestTaskMPI::RunImpl() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  auto multiply = [this](bool row_major) {
    for (int i = 0; i < rc_size_; ++i) {
      for (int j = 0; j < rc_size_; ++j) {
        int sum = 0;
        for (int k = 0; k < rc_size_; ++k) {
          int a = input_[(row_major ? i : k) * rc_size_ + (row_major ? k : i)];
          int b = input_[(row_major ? k : j) * rc_size_ + (row_major ? j : k)];
          sum += a * b;
        }
        output_[(i * rc_size_) + j] += sum;
      }
    }
  };

  multiply(rank == 0);
  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool nesterov_a_test_task_mpi::TestTaskMPI::PostProcessingImpl() {
  for (size_t i = 0; i < output_.size(); i++) {
    reinterpret_cast<int *>(task_data->outputs[0])[i] = output_[i];
  }
  return true;
}
