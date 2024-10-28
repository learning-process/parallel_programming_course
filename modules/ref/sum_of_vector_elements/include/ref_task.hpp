#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType>
class SumOfVectorElements : public ppc::core::Task {
 public:
  explicit SumOfVectorElements(ppc::core::TaskDataPtr taskData_) : Task(taskData_) {}
  bool pre_processing_impl() override {
    // Init vectors
    input_ = std::vector<InOutType>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(taskData->inputs[0]);
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    sum = 0;
    return true;
  }

  bool validation_impl() override {
    // Check count elements of output
    return taskData->outputs_count[0] == 1;
  }

  bool run_impl() override {
    sum = std::accumulate(input_.begin(), input_.end(), 0);
    return true;
  }

  bool post_processing_impl() override {
    reinterpret_cast<InOutType*>(taskData->outputs[0])[0] = sum;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType sum;
};

}  // namespace ppc::reference
