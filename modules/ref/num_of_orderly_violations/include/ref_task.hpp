#ifndef MODULES_REFERENCE_NUM_OF_ORDERLY_VIOLATIONS_REF_TASK_HPP_
#define MODULES_REFERENCE_NUM_OF_ORDERLY_VIOLATIONS_REF_TASK_HPP_

#include <algorithm>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType, class CountType>
class NumOfOrderlyViolations : public ppc::core::Task {
 public:
  explicit NumOfOrderlyViolations(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InOutType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    // Init value for output
    num_ = 0;
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return task_data->outputs_count[0] == 1;
  }

  bool RunImpl() override {
    auto rotate_in = input_;
    int rot_left = 1;
    rotate(rotate_in.begin(), rotate_in.begin() + rot_left, rotate_in.end());

    auto temp_res = std::vector<bool>(input_.size());
    std::transform(input_.begin(), input_.end(), rotate_in.begin(), temp_res.begin(),
                   [](InOutType x, InOutType y) { return x > y; });

    num_ = std::count_if(temp_res.begin(), temp_res.end() - 1, [](InOutType elem) { return elem; });
    return true;
  }

  bool PostProcessingImpl() override {
    reinterpret_cast<CountType*>(task_data->outputs[0])[0] = num_;
    return true;
  }

 private:
  std::vector<InOutType> input_;
  CountType num_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_NUM_OF_ORDERLY_VIOLATIONS_REF_TASK_HPP_
