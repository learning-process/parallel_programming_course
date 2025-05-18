#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_seq {

class TestTaskSequential : public ppc::core::Task {
 public:
  explicit TestTaskSequential(const std::vector<int>& in) : input_(in) {}
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  std::vector<int> Get();
  std::vector<int> input_, output_;

 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task_seq