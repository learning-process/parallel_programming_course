#pragma once

#include "example/common/include/common.hpp"
#include "task/include/task.hpp"

namespace example_processes_t3 {

class NesterovATestTaskSEQ : public BaseTask<ppc::task::TypeOfTask::kSEQ> {
 public:
  explicit NesterovATestTaskSEQ(const InType &in);

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace example_processes_t3
