// Copyright 2023 Nesterov Alexander
#pragma once

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_omp {

std::vector<int> getRandomVector(int sz);

class TestOMPTaskSequential : public ppc::core::Task {
 public:
  explicit TestOMPTaskSequential(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
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

class TestOMPTaskParallel : public ppc::core::Task {
 public:
  explicit TestOMPTaskParallel(std::shared_ptr<ppc::core::TaskData> taskData_, std::string ops_)
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

}  // namespace nesterov_a_test_task_omp