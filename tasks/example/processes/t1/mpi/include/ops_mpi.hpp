#pragma once

#include "example/common/include/common.hpp"
#include "task/include/task.hpp"

namespace example_processes_t1 {

class NesterovATestTaskMPI : public BaseTask<ppc::task::TypeOfTask::kMPI> {
 public:
  explicit NesterovATestTaskMPI(const InType &in);

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace example_processes_t1
