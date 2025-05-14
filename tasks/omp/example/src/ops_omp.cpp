#include "omp/example/include/ops_omp.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

bool nesterov_a_test_task_omp::TestTaskOpenMP::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_omp::TestTaskOpenMP::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_omp::TestTaskOpenMP::RunImpl() {
#pragma omp parallel default(none)
  {
#pragma omp critical
    {
      // Multiply matrices
      for (int i = 0; i < rc_size_; ++i) {
        for (int j = 0; j < rc_size_; ++j) {
          output_[(i * rc_size_) + j] = 0;
          for (int k = 0; k < rc_size_; ++k) {
            output_[(i * rc_size_) + j] += input_[(i * rc_size_) + k] * input_[(k * rc_size_) + j];
          }
        }
      }
    }
  }
  return true;
}

bool nesterov_a_test_task_omp::TestTaskOpenMP::PostProcessingImpl() { return true; }
