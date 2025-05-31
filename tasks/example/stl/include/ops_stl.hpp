#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"
#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

class NesterovATestTaskSTL : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kSTL; }
  explicit NesterovATestTaskSTL(const InType& in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task
