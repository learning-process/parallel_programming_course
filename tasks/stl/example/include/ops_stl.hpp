#ifndef TASKS_EXAMPLES_TEST_STD_OPS_STD_H_
#define TASKS_EXAMPLES_TEST_STD_OPS_STD_H_

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_stl {

class TestSTLTaskSequential : public ppc::core::Task {
 public:
  explicit TestSTLTaskSequential(ppc::core::task_dataPtr task_data, std::string ops_)
      : Task(std::move(task_data)), ops_(std::move(ops_)) {}
  bool PreProcessingImpl() override;
  bool ValidationImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  std::vector<int> input_;
  int res_{};
  std::string ops_;
};

class TestSTLTaskParallel : public ppc::core::Task {
 public:
  explicit TestSTLTaskParallel(ppc::core::task_dataPtr task_data, std::string ops_)
      : Task(std::move(task_data)), ops_(std::move(ops_)) {}
  bool PreProcessingImpl() override;
  bool ValidationImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  std::vector<int> input_;
  int res_{};
  std::string ops_;
};

}  // namespace nesterov_a_test_task_stl

#endif  // TASKS_EXAMPLES_TEST_STD_OPS_STD_H_
