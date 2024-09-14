// Copyright 2023 Nesterov Alexander
#ifndef TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
#define TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_tbb {

std::vector<int> getRandomVector(int sz);

class TestTBBTaskSequential : public ppc::core::Task {
 public:
  explicit TestTBBTaskSequential(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
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

class TestTBBTaskParallel : public ppc::core::Task {
 public:
  explicit TestTBBTaskParallel(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
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

}  // namespace nesterov_a_test_task_tbb

#endif  // TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
