#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"
#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

class NesterovATestTaskOMP : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kOMP; }
  explicit NesterovATestTaskOMP(const InType& in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task
