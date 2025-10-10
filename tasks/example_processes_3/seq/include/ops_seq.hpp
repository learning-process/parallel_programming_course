#pragma once

#include "example_processes_3/common/include/common.hpp"
#include "task/include/task.hpp"

namespace nesterov_a_test_task_processes_3 {

class NesterovATestTaskSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit NesterovATestTaskSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task_processes_3
