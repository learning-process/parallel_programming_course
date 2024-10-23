// Copyright 2023 Nesterov Alexander
#pragma once

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_seq {

class TestTaskSequential : public ppc::core::Task {
 public:
  explicit TestTaskSequential(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(std::move(taskData_)) {}
  bool pre_processing_impl() override;
  bool validation_impl() override;
  bool run_impl() override;
  bool post_processing_impl() override;

 private:
  int input_{}, res{};
};

}  // namespace nesterov_a_test_task_seq