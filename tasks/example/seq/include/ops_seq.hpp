#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"
#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

class NesterovATestTaskSEQ : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetTypeOfTask() { return ppc::core::TypeOfTask::kSEQ; }
  explicit NesterovATestTaskSEQ(const InType& in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task
