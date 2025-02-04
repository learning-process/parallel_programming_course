#ifndef MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InType, class OutType>
class AverageOfVectorElements : public ppc::core::Task {
 public:
  explicit AverageOfVectorElements(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    average_ = 0.0;
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return task_data->outputs_count[0] == 1;
  }

  bool RunImpl() override {
    average_ = static_cast<OutType>(std::accumulate(input_.begin(), input_.end(), 0.0));
    average_ /= static_cast<OutType>(task_data->inputs_count[0]);
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<OutType*>(task_data->outputs[0])[0] = average_;
    return true;
  }

 private:
  std::vector<InType> input_;
  OutType average_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
