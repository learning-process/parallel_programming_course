#ifndef TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
#define TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_tbb {

class TestTBBTaskSequential : public ppc::core::Task {
 public:
  explicit TestTBBTaskSequential(ppc::core::TaskDataPtr taskData_, std::string ops_)
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

class TestTBBTaskParallel : public ppc::core::Task {
 public:
  explicit TestTBBTaskParallel(ppc::core::TaskDataPtr taskData_, std::string ops_)
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

}  // namespace nesterov_a_test_task_tbb

#endif  // TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
