// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_SUM_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
#define MODULES_REFERENCE_SUM_OF_VECTOR_ELEMENTS_REF_TASK_HPP_

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <numeric>
#include "core/include/task.hpp"

namespace ppc {
namespace reference {

template<class T>
class SumOfVectorElements : public ppc::core::Task {
 public:
    explicit SumOfVectorElements(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
    bool pre_processing() override {
        // Init vectors
        input_ = std::vector<T>(taskData->inputs_count[0]);
        auto tmp_ptr = reinterpret_cast<T*>(taskData->inputs[0]);
        for (int i = 0; i < taskData->inputs_count[0]; i++) {
            input_[i] = tmp_ptr[i];
        }
        // Init value for output
        sum = 0;
        return true;
    }

    bool validation() override {
        // Check count elements of output
        if (taskData->outputs_count[0] == 1) {
            return true;
        } else {
            return false;
        }
    }

    bool run() override {
        sum = std::accumulate(input_.begin(), input_.end(), 0);
        return true;
    }

    bool post_processing() override {
        reinterpret_cast<T*>(taskData->outputs[0])[0] = sum;
        return true;
    }

 private:
    std::vector<T> input_;
    T sum;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_SUM_OF_VECTOR_ELEMENTS_REF_TASK_HPP_
