#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"
#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

void MultiplyRowMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size);
void MultiplyColumnMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size);

class NesterovATestTaskMPI : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetTypeOfTask() { return ppc::core::TypeOfTask::kMPI; }
  explicit NesterovATestTaskMPI(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int rc_size_{};

  void MultiplyMatrixBasedOnRank();
};

}  // namespace nesterov_a_test_task
