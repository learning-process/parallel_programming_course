#include "stl/example/include/ops_stl.hpp"

#include <cmath>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>

#include "core/util/include/util.hpp"

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

bool nesterov_a_test_task_stl::TestTaskSTL::ValidationImpl() {
  auto sqrt_size = static_cast<int>(std::sqrt(input_.size()));
  return sqrt_size * sqrt_size == static_cast<int>(input_.size());
}

bool nesterov_a_test_task_stl::TestTaskSTL::PreProcessingImpl() {
  rc_size_ = static_cast<int>(std::sqrt(input_.size()));
  output_ = std::vector<int>(input_.size(), 0);
  return true;
}

bool nesterov_a_test_task_stl::TestTaskSTL::RunImpl() {
  const int num_threads = ppc::util::GetPPCNumThreads();
  std::vector<std::thread> threads(num_threads);
  for (int i = 0; i < num_threads; i++) {
    threads[i] = std::thread(MatMul, std::cref(input_), rc_size_, std::ref(output_));
    threads[i].join();
  }
  return true;
}

bool nesterov_a_test_task_stl::TestTaskSTL::PostProcessingImpl() { return true; }
