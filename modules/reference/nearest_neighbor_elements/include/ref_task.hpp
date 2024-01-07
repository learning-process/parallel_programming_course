// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace reference {

template <class InOutType, class IndexType>
class NearestNeighborElements : public ppc::core::Task {
 public:
  explicit NearestNeighborElements(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
  bool pre_processing() override {
    internal_order_test();
    // Init vectors
    input_ = std::vector<InOutType>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(taskData->inputs[0]);
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    l_elem = r_elem = 0;
    l_elem_index = r_elem_index = 0;
    return true;
  }

  bool validation() override {
    internal_order_test();
    // Check count elements of output
    return taskData->outputs_count[0] == 2 && taskData->outputs_count[1] == 2;
  }

  bool run() override {
    internal_order_test();
    auto rotate_in = input_;
    int rot_left = 1;
    rotate(rotate_in.begin(), rotate_in.begin() + rot_left, rotate_in.end());

    auto temp_res = std::vector<InOutType>(input_.size());
    std::transform(input_.begin(), input_.end(), rotate_in.begin(), temp_res.begin(),
                   [](InOutType x, InOutType y) { return std::abs(x - y); });

    auto result = std::min_element(temp_res.begin(), temp_res.end() - 1);
    l_elem_index = static_cast<IndexType>(std::distance(temp_res.begin(), result));
    l_elem = input_[l_elem_index];

    r_elem_index = l_elem_index + 1;
    r_elem = input_[r_elem_index];
    return true;
  }

  bool post_processing() override {
    internal_order_test();
    reinterpret_cast<InOutType*>(taskData->outputs[0])[0] = l_elem;
    reinterpret_cast<InOutType*>(taskData->outputs[0])[1] = r_elem;
    reinterpret_cast<IndexType*>(taskData->outputs[1])[0] = l_elem_index;
    reinterpret_cast<IndexType*>(taskData->outputs[1])[1] = r_elem_index;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType l_elem, r_elem;
  IndexType l_elem_index, r_elem_index;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_
