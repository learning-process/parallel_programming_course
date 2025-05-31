#pragma once

#include <utility>
#include <vector>

#include "example/common/include/common.hpp"

namespace nesterov_a_test_task {

void MatMul(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec);
void MatMulTBB(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec);

class NesterovATestTaskALL : public BaseTask {
 public:
  static constexpr ppc::core::TypeOfTask GetStaticTypeOfTask() { return ppc::core::TypeOfTask::kALL; }
  explicit NesterovATestTaskALL(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int rc_size_{};
};

}  // namespace nesterov_a_test_task
