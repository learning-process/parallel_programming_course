#ifndef MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_

#include <algorithm>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType, class IndexType>
class NearestNeighborElements : public ppc::core::Task {
 public:
  explicit NearestNeighborElements(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InOutType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    l_elem_ = r_elem_ = 0;
    l_elem_index_ = r_elem_index_ = 0;
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return task_data->outputs_count[0] == 2 && task_data->outputs_count[1] == 2;
  }

  bool RunImpl() override {
    auto rotate_in = input_;
    int rot_left = 1;
    rotate(rotate_in.begin(), rotate_in.begin() + rot_left, rotate_in.end());

    auto temp_res = std::vector<InOutType>(input_.size());
    std::transform(input_.begin(), input_.end(), rotate_in.begin(), temp_res.begin(),
                   [](InOutType x, InOutType y) { return std::abs(x - y); });

    auto result = std::min_element(temp_res.begin(), temp_res.end() - 1);
    l_elem_index_ = static_cast<IndexType>(std::distance(temp_res.begin(), result));
    l_elem_ = input_[l_elem_index_];

    r_elem_index_ = l_elem_index_ + 1;
    r_elem_ = input_[r_elem_index_];
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<InOutType*>(task_data->outputs[0])[0] = l_elem_;
    reinterpret_cast<InOutType*>(task_data->outputs[0])[1] = r_elem_;
    reinterpret_cast<IndexType*>(task_data->outputs[1])[0] = l_elem_index_;
    reinterpret_cast<IndexType*>(task_data->outputs[1])[1] = r_elem_index_;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType l_elem_, r_elem_;
  IndexType l_elem_index_, r_elem_index_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_NEAREST_NEIGHBOR_ELEMENTS_REF_TASK_HPP_
