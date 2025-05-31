#include "example/omp/include/ops_omp.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

namespace nesterov_a_test_task {

NesterovATestTaskOMP::NesterovATestTaskOMP(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool NesterovATestTaskOMP::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(GetInput().size()));
  return sqrt_size * sqrt_size == static_cast<int>(GetInput().size());
}

bool NesterovATestTaskOMP::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(GetInput().size()));
  GetOutput() = OutType(GetInput().size(), 0);
  return true;
}

bool NesterovATestTaskOMP::RunImpl() {
#pragma omp parallel default(none)
  {
#pragma omp critical
    {
      // Multiply matrices
      for (int i = 0; i < rc_size_; ++i) {
        for (int j = 0; j < rc_size_; ++j) {
          GetOutput()[(i * rc_size_) + j] = 0;
          for (int k = 0; k < rc_size_; ++k) {
            GetOutput()[(i * rc_size_) + j] += GetInput()[(i * rc_size_) + k] * GetInput()[(k * rc_size_) + j];
          }
        }
      }
    }
  }
  return true;
}

bool NesterovATestTaskOMP::PostProcessingImpl() { return true; }

}  // namespace nesterov_a_test_task
