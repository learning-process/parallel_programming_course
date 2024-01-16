// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_
#define MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <numeric>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace reference {

template <class InOutType, class IndexType>
class SumValuesByRowsMatrix : public ppc::core::Task {
 public:
  explicit SumValuesByRowsMatrix(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
  bool pre_processing() override {
    internal_order_test();
    // Init vectors
    input_ = std::vector<InOutType>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<InOutType*>(taskData->inputs[0]);
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    rows = reinterpret_cast<IndexType*>(taskData->inputs[1])[0];
    cols = reinterpret_cast<IndexType*>(taskData->inputs[1])[1];

    // Init value for output
    sum_ = std::vector<InOutType>(cols, 0.f);
    return true;
  }

  bool validation() override {
    internal_order_test();
    // Check count elements of output
    return static_cast<bool>(taskData->inputs_count[1] == 2 &&
                             taskData->outputs_count[0] == reinterpret_cast<IndexType*>(taskData->inputs[1])[0]);
  }

  bool run() override {
    internal_order_test();
    for (size_t i = 0; i < rows; i++) {
      sum_[i] = std::accumulate(input_.begin() + cols * i, input_.begin() + cols * (i + 1), 0.f);
    }
    return true;
  }

  bool post_processing() override {
    internal_order_test();
    for (IndexType i = 0; i < rows; i++) {
      reinterpret_cast<InOutType*>(taskData->outputs[0])[i] = sum_[i];
    }
    return true;
  }

 private:
  std::vector<InOutType> input_;
  IndexType rows, cols;
  std::vector<InOutType> sum_;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_SUM_VALUES_BY_ROWS_MATRIX_REF_TASK_HPP_
