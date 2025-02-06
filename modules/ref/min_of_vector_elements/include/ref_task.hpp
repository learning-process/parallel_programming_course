#ifndef MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <algorithm>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType, class IndexType>
class MinOfVectorElements : public ppc::core::Task {
 public:
  explicit MinOfVectorElements(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InOutType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    min_ = 0.0;
    min_index_ = 0;
    return true;
  }

  bool ValidationImpl() override {
    bool is_count_values_correct = false;
    bool is_count_indexes_correct = false;
    // Check count elements of output
    is_count_values_correct = task_data->outputs_count[0] == 1;
    is_count_indexes_correct = task_data->outputs_count[1] == 1;

    return is_count_values_correct && is_count_indexes_correct;
  }

  bool RunImpl() override {
    auto result = std::min_element(input_.begin(), input_.end());
    min_ = static_cast<InOutType>(*result);
    min_index_ = static_cast<IndexType>(std::distance(input_.begin(), result));
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<InOutType*>(task_data->outputs[0])[0] = min_;
    reinterpret_cast<IndexType*>(task_data->outputs[1])[0] = min_index_;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType min_;
  IndexType min_index_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
