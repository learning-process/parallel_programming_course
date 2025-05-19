#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_mpi {

void MultiplyRowMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size);
void MultiplyColumnMajor(const std::vector<int> &in, std::vector<int> &out, int rc_size);

using InType = std::vector<int>;
using OutType = std::vector<int>;

class TestTaskMPI : public ppc::core::Task<InType, OutType> {
 public:
  explicit TestTaskMPI(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
 private:
  int rc_size_{};

  void MultiplyMatrixBasedOnRank();
};

}  // namespace nesterov_a_test_task_mpi
