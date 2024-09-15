// Copyright 2023 Nesterov Alexander
#ifndef TASKS_EXAMPLES_TEST_STD_OPS_STD_H_
#define TASKS_EXAMPLES_TEST_STD_OPS_STD_H_

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_stl {

std::vector<int> getRandomVector(int sz);

class TestSTLTaskSequential : public ppc::core::Task {
 public:
  explicit TestSTLTaskSequential(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<int> input_;
  int res{};
  std::string ops;
};

class TestSTLTaskParallel : public ppc::core::Task {
 public:
  explicit TestSTLTaskParallel(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<int> input_;
  int res{};
  std::string ops;
};

}  // namespace nesterov_a_test_task_stl

#endif  // TASKS_EXAMPLES_TEST_STD_OPS_STD_H_
