#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_all {

void MatMul(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec);
void MatMulTBB(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec);

using InType = std::vector<int>;
using OutType = std::vector<int>;

class TestTaskALL : public ppc::core::Task<InType, OutType> {
 public:
  explicit TestTaskALL(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task_all
