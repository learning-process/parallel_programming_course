#ifndef MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_
#define MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_

#include <cstddef>
#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::reference {

template <class InOutType, class IndexType>
class SumValuesByRowsMatrix : public ppc::core::Task {
 public:
  explicit SumValuesByRowsMatrix(ppc::core::TaskDataPtr task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    // Init vectors
    input_ = std::vector<InOutType>(task_data->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    rows_ = reinterpret_cast<IndexType*>(task_data->inputs[1])[0];
    cols_ = reinterpret_cast<IndexType*>(task_data->inputs[1])[1];

    // Init value for output
    sum_ = std::vector<InOutType>(cols_, 0.F);
    return true;
  }

  bool ValidationImpl() override {
    // Check count elements of output
    return static_cast<bool>(task_data->inputs_count[1] == 2 &&
                             task_data->outputs_count[0] == reinterpret_cast<IndexType*>(task_data->inputs[1])[0]);
  }

  bool RunImpl() override {
    for (size_t i = 0; i < rows_; i++) {
      sum_[i] = std::accumulate(input_.begin() + (cols_ * i), input_.begin() + (cols_ * (i + 1)), 0.F);
    }
    return true;
  }

  bool PostProcessingImpl() override {
    for (IndexType i = 0; i < rows_; i++) {
      reinterpret_cast<InOutType*>(task_data->outputs[0])[i] = sum_[i];
    }
    return true;
  }

 private:
  std::vector<InOutType> input_;
  IndexType rows_, cols_;
  std::vector<InOutType> sum_;
};

}  // namespace ppc::reference

#endif  // MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_
