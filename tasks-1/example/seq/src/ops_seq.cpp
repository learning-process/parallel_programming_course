#include "example/seq/include/ops_seq.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

nesterov_a_test_task_seq::TestTaskSEQ::TestTaskSEQ(const InType& in) {
  GetInput() = in;
}

bool nesterov_a_test_task_seq::TestTaskSEQ::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(GetInput().size()));
  return sqrt_size * sqrt_size == static_cast<int>(GetInput().size());
}

bool nesterov_a_test_task_seq::TestTaskSEQ::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(GetInput().size()));
  GetOutput() = OutType(GetInput().size(), 0);
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSEQ::RunImpl() {
  // Multiply matrices
  for (int i = 0; i < rc_size_; ++i) {
    for (int j = 0; j < rc_size_; ++j) {
      for (int k = 0; k < rc_size_; ++k) {
        GetOutput()[(i * rc_size_) + j] += GetInput()[(i * rc_size_) + k] * GetInput()[(k * rc_size_) + j];
      }
    }
  }
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSEQ::PostProcessingImpl() { return true; }
