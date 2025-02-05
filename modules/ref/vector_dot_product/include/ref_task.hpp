#ifndef MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_
#define MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_

#include <cstddef>
#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType>
class VectorDotProduct : public ppc::core::Task {
 public:
  explicit VectorDotProduct(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<std::vector<InOutType> >(2);
    for (size_t i = 0; i < input_.size(); i++) {
      input_[i] = std::vector<InOutType>(task_data->inputs_count[i]);
      auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[i]);
      for (unsigned j = 0; j < task_data->inputs_count[i]; j++) {
        input_[i][j] = tmp_ptr[j];
      }
    }

    // Init value for output
    dor_product_ = 0;
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return task_data->outputs_count[0] == 1 && task_data->inputs_count[0] == task_data->inputs_count[1];
  }

  bool RunImpl() override {
    dor_product_ = std::inner_product(input_[0].begin(), input_[0].end(), input_[1].begin(), 0.0);
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<InOutType*>(task_data->outputs[0])[0] = dor_product_;
    return true;
  }

 private:
  std::vector<std::vector<InOutType> > input_;
  InOutType dor_product_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_
