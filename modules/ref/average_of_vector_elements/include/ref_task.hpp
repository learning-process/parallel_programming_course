#ifndef MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace reference {

template <class InType, class OutType>
class AverageOfVectorElements : public ppc::core::Task {
 public:
  explicit AverageOfVectorElements(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
  bool pre_processing_impl() override {
    // Init vectors
    input_ = std::vector<InType>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InType*>(taskData->inputs[0]);
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    average = 0.0;
    return true;
  }

  bool validation_impl() override {
    // Check count elements of output
    return taskData->outputs_count[0] == 1;
  }

  bool run_impl() override {
    average = static_cast<OutType>(std::accumulate(input_.begin(), input_.end(), 0.0));
    average /= static_cast<OutType>(taskData->inputs_count[0]);
    return true;
  }

  bool post_processing_impl() override {
    reinterpret_cast<OutType*>(taskData->outputs[0])[0] = average;
    return true;
  }

 private:
  std::vector<InType> input_;
  OutType average;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
