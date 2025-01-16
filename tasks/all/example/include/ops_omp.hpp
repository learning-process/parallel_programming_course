#pragma once

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_omp {

class TestOMPTaskSequential : public ppc::core::Task {
 public:
  explicit TestOMPTaskSequential(ppc::core::TaskDataPtr task_data, std::string ops)
      : Task(std::move(task_data)), ops_(std::move(ops)) {}
  bool PreProcessingImpl() override;
  bool ValidationImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  std::vector<int> input_;
  int res_{};
  std::string ops_;
};

class TestOMPTaskParallel : public ppc::core::Task {
 public:
  explicit TestOMPTaskParallel(ppc::core::TaskDataPtr task_data, std::string ops)
      : Task(std::move(task_data)), ops_(std::move(ops)) {}
  bool PreProcessingImpl() override;
  bool ValidationImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  std::vector<int> input_;
  int res_{};
  std::string ops_;
};

}  // namespace nesterov_a_test_task_omp