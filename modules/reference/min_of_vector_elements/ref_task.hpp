// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include "core/include/task.hpp"

namespace ppc {
namespace reference {

template<class InOutType, class IndexType>
class MinOfVectorElements : public ppc::core::Task {
 public:
    explicit MinOfVectorElements(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
    bool pre_processing() override {
        internal_order_test();
        // Init vectors
        input_ = std::vector<InOutType>(taskData->inputs_count[0]);
        auto tmp_ptr = reinterpret_cast<InOutType*>(taskData->inputs[0]);
        for (int i = 0; i < taskData->inputs_count[0]; i++) {
            input_[i] = tmp_ptr[i];
        }
        // Init value for output
        min = 0.0;
        min_index = 0;
        return true;
    }

    bool validation() override {
        internal_order_test();
        bool isCountValuesCorrect, isCountIndexesCorrect;
        // Check count elements of output
        if (taskData->outputs_count[0] == 1) {
            isCountValuesCorrect = true;
        } else {
            isCountValuesCorrect = false;
        }
        if (taskData->outputs_count[1] == 1) {
            isCountIndexesCorrect = true;
        } else {
            isCountIndexesCorrect = false;
        }

        if (isCountValuesCorrect && isCountIndexesCorrect) {
            return true;
        } else {
            return false;
        }
    }

    bool run() override {
        internal_order_test();
        auto result = std::min_element(input_.begin(), input_.end());
        min = static_cast<InOutType>(*result);
        min_index = static_cast<IndexType>(std::distance(input_.begin(), result));
        return true;
    }

    bool post_processing() override {
        internal_order_test();
        reinterpret_cast<InOutType*>(taskData->outputs[0])[0] = min;
        reinterpret_cast<IndexType*>(taskData->outputs[1])[0] = min_index;
        return true;
    }

 private:
    std::vector<InOutType> input_;
    InOutType min;
    IndexType min_index;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_MIN_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
