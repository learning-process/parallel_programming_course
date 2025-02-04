#pragma once

#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType>
class SumOfVectorElements : public ppc::core::Task {
 public:
  explicit SumOfVectorElements(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InOutType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    sum_ = 0;
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return task_data->outputs_count[0] == 1;
  }

  bool RunImpl() override {
    sum_ = std::accumulate(input_.begin(), input_.end(), 0);
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<InOutType*>(task_data->outputs[0])[0] = sum_;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType sum_;
};

}  // namespace ppc::reference
