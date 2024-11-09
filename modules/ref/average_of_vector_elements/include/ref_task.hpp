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
class AverageOfVectorElements : public ppc::core::Task<InType, OutType> {
 public:
  explicit AverageOfVectorElements(typename AverageOfVectorElements::TaskData taskData_)
      : AverageOfVectorElements::Task(taskData_) {}

 protected:
  bool pre_processing_impl() override {
    // Init vectors
    const auto& src = this->taskData.input;
    input_.assign(src.begin(), src.end());
    // Init value for output
    average = 0.0;
    return true;
  }

  bool validation_impl() override {
    // Check count elements of output
    return this->taskData.output.size() == 1;
  }

  bool run_impl() override {
    average = static_cast<OutType>(std::accumulate(input_.begin(), input_.end(), 0.0));
    average /= static_cast<OutType>(input_.size());
    return true;
  }

  bool post_processing_impl() override {
    this->taskData.output[0] = average;
    return true;
  }

 private:
  std::vector<InType> input_;
  OutType average;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_AVERAGE_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
