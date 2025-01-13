#pragma once

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_seq {

class TestTaskSequential : public ppc::core::Task {
 public:
  explicit TestTaskSequential(ppc::core::task_dataPtr task_data) : Task(std::move(task_data)) {}
  bool PreProcessingImpl() override;
  bool ValidationImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int input_{}, res_{};
};

}  // namespace nesterov_a_test_task_seq