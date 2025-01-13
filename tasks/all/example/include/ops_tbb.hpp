#ifndef TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
#define TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_

#include <string>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_tbb {

class TestTBBTaskSequential : public ppc::core::Task {
 public:
  explicit TestTBBTaskSequential(ppc::core::TaskDataPtr task_data, std::string ops)
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

class TestTBBTaskParallel : public ppc::core::Task {
 public:
  explicit TestTBBTaskParallel(ppc::core::TaskDataPtr task_data, std::string ops)
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

}  // namespace nesterov_a_test_task_tbb

#endif  // TASKS_EXAMPLES_TEST_TBB_OPS_TBB_H_
