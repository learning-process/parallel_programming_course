#pragma once

#include <vector>

#include "core/task/include/task.hpp"
#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

class NesterovATestTaskMPI : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kMPI; }
  explicit NesterovATestTaskMPI(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task
