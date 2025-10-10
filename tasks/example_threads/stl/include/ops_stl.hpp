#pragma once

#include "example_threads/common/include/common.hpp"
#include "task/include/task.hpp"

namespace nesterov_a_test_task_threads {

class NesterovATestTaskSTL : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSTL;
  }
  explicit NesterovATestTaskSTL(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task_threads
