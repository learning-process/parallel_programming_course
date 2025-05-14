#include "seq/example/include/ops_seq.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

bool nesterov_a_test_task_seq::TestTaskSequential::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_seq::TestTaskSequential::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::RunImpl() {
  // Multiply matrices
  for (int i = 0; i < rc_size_; ++i) {
    for (int j = 0; j < rc_size_; ++j) {
      for (int k = 0; k < rc_size_; ++k) {
        output_[(i * rc_size_) + j] += input_[(i * rc_size_) + k] * input_[(k * rc_size_) + j];
      }
    }
  }
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::PostProcessingImpl() { return true; }
