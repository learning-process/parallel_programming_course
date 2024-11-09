#ifndef MODULES_REFERENCE_MAX_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_MAX_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace reference {

template <class InOutType, class IndexType>
class MaxOfVectorElements : public ppc::core::Task<InOutType, std::pair<IndexType, InOutType>> {
 public:
  explicit MaxOfVectorElements(typename MaxOfVectorElements::TaskData taskData_)
      : MaxOfVectorElements::Task(taskData_) {}

 protected:
  bool pre_processing_impl() override {
    // Init vectors
    const auto& src = this->taskData.input;
    input_.assign(src.begin(), src.end());
    // Init value for output
    max = 0.0;
    max_index = 0;
    return true;
  }

  bool validation_impl() override {
    // Check count elements of output
    return this->taskData.output.size() == 1;
  }

  bool run_impl() override {
    auto result = std::max_element(input_.begin(), input_.end());
    max = static_cast<InOutType>(*result);
    max_index = static_cast<IndexType>(std::distance(input_.begin(), result));
    return true;
  }

  bool post_processing_impl() override {
    this->taskData.output[0] = std::make_pair(max_index, max);
    return true;
  }

 private:
  std::vector<InOutType> input_;
  InOutType max;
  IndexType max_index;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_MAX_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
