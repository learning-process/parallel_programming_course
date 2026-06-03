#pragma once

#include "example/common/include/common.hpp"
#include "task/include/task.hpp"

namespace example_processes_t3 {

class NesterovATestTaskMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit NesterovATestTaskMPI(const InType &in);

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace example_processes_t3
