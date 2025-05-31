#include "example/seq/include/ops_seq.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

namespace nesterov_a_test_task {

NesterovATestTaskSEQ::NesterovATestTaskSEQ(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool NesterovATestTaskSEQ::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(GetInput().size()));
  return sqrt_size * sqrt_size == static_cast<int>(GetInput().size());
}

bool NesterovATestTaskSEQ::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(GetInput().size()));
  GetOutput() = OutType(GetInput().size(), 0);
  return true;
}

bool NesterovATestTaskSEQ::RunImpl() {
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

bool NesterovATestTaskSEQ::PostProcessingImpl() { return true; }

}  // namespace nesterov_a_test_task
