#pragma once

#include "example/common/include/common.hpp"
#include "task/include/task.hpp"

namespace example_threads {

class NesterovATestTaskTBB : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kTBB;
  }
  explicit NesterovATestTaskTBB(const InType &in);

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace example_threads
