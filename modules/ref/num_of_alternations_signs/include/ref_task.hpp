// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_NUM_OF_ALTERNATIONS_SIGNS_REF_TASK_HPP_
#define MODULES_REFERENCE_NUM_OF_ALTERNATIONS_SIGNS_REF_TASK_HPP_

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace reference {

template <class InOutType, class CountType>
class NumOfAlternationsSigns : public ppc::core::Task {
 public:
  explicit NumOfAlternationsSigns(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
  bool pre_processing() override {
    internal_order_test();
    // Init vectors
    input_ = std::vector<InOutType>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(taskData->inputs[0]);
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    num = 0;
    return true;
  }

  bool validation() override {
    internal_order_test();
    // Check count elements of output
    return taskData->outputs_count[0] == 1;
  }

  bool run() override {
    internal_order_test();
    auto rotate_in = input_;
    int rot_left = 1;
    rotate(rotate_in.begin(), rotate_in.begin() + rot_left, rotate_in.end());

    auto temp_res = input_;
    std::transform(input_.begin(), input_.end(), rotate_in.begin(), temp_res.begin(), std::multiplies<>());

    num = std::count_if(temp_res.begin(), temp_res.end() - 1, [](InOutType elem) { return elem < 0; });
    return true;
  }

  bool post_processing() override {
    internal_order_test();
    reinterpret_cast<CountType*>(taskData->outputs[0])[0] = num;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  CountType num;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_NUM_OF_ALTERNATIONS_SIGNS_REF_TASK_HPP_
