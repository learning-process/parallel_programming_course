#include "example/tbb/include/ops_tbb.hpp"

#include <tbb/tbb.h>

#include <cmath>
#include <core/util/include/util.hpp>
#include <cstddef>
#include <vector>

#include "oneapi/tbb/parallel_for.h"

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

bool nesterov_a_test_task_tbb::TestTaskTBB::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_tbb::TestTaskTBB::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_tbb::TestTaskTBB::RunImpl() {
  tbb::parallel_for(0, ppc::util::GetPPCNumThreads(), [&](int i) { MatMul(input_, rc_size_ - i, output_); });
  MatMul(input_, rc_size_, output_);
  return true;
}

bool nesterov_a_test_task_tbb::TestTaskTBB::PostProcessingImpl() { return true; }

std::vector<int> nesterov_a_test_task_tbb::TestTaskTBB::Get() { return output_; }
