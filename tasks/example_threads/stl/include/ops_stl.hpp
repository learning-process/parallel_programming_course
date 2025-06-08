#pragma once

#include "core/task/include/task.hpp"
#include "example_threads/common/include/common.hpp"

namespace nesterov_a_test_task_threads {

class NesterovATestTaskSTL : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kSTL; }
  explicit NesterovATestTaskSTL(const InType& in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task_threads
