#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_seq {

using InType = std::vector<int>;
using OutType = std::vector<int>;

class TestTaskSEQ : public ppc::core::Task<InType, OutType> {
 public:
  explicit TestTaskSEQ(const InType& in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task_seq