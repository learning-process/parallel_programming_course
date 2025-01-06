#pragma once

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_omp {

class TestOMPTaskSequential : public ppc::core::Task {
 public:
  explicit TestOMPTaskSequential(ppc::core::TaskDataPtr taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing_impl() override;
  bool validation_impl() override;
  bool run_impl() override;
  bool post_processing_impl() override;

 private:
  std::vector<int> input_;
  int res{};
  std::string ops;
};

class TestOMPTaskParallel : public ppc::core::Task {
 public:
  explicit TestOMPTaskParallel(ppc::core::TaskDataPtr taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing_impl() override;
  bool validation_impl() override;
  bool run_impl() override;
  bool post_processing_impl() override;

 private:
  std::vector<int> input_;
  int res{};
  std::string ops;
};

}  // namespace nesterov_a_test_task_omp