#include "example/tbb/include/ops_tbb.hpp"

#include <tbb/tbb.h>

#include <cmath>
#include <core/util/include/util.hpp>
#include <cstddef>
#include <vector>

#include "oneapi/tbb/parallel_for.h"

namespace nesterov_a_test_task {

namespace {
void MatMul(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec) {
  for (int i = 0; i < rc_size; ++i) {
    for (int j = 0; j < rc_size; ++j) {
      out_vec[(i * rc_size) + j] = 0;
      for (int k = 0; k < rc_size; ++k) {
        out_vec[(i * rc_size) + j] += in_vec[(i * rc_size) + k] * in_vec[(k * rc_size) + j];
      }
    }
  }
}
}  // namespace

NesterovATestTaskTBB::NesterovATestTaskTBB(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool NesterovATestTaskTBB::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(GetInput().size()));
  return sqrt_size * sqrt_size == static_cast<int>(GetInput().size());
}

bool NesterovATestTaskTBB::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(GetInput().size()));
  GetOutput() = OutType(GetInput().size(), 0);
  return true;
}

bool NesterovATestTaskTBB::RunImpl() {
  tbb::parallel_for(0, ppc::util::GetNumThreads(), [&](int i) { MatMul(GetInput(), rc_size_ - i, GetOutput()); });
  MatMul(GetInput(), rc_size_, GetOutput());
  return true;
}

bool NesterovATestTaskTBB::PostProcessingImpl() { return true; }

}  // namespace nesterov_a_test_task
