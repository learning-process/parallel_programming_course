#pragma once

#include <vector>

#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

class NesterovATestTaskALL : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kALL; }
  explicit NesterovATestTaskALL(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nesterov_a_test_task
