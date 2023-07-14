// Copyright 2023 Nesterov Alexander

#ifndef MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_
#define MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <numeric>
#include "core/include/task.hpp"

namespace ppc {
namespace reference {

template<class InOutType>
class VectorDotProduct : public ppc::core::Task {
 public:
    explicit VectorDotProduct(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
    bool pre_processing() override {
        // Init vectors
        input_ = std::vector< std::vector<InOutType> >(2);
        for (int i = 0; i < input_.size(); i++) {
            input_[i] = std::vector<InOutType>(taskData->inputs_count[i]);
            auto tmp_ptr_ = reinterpret_cast<InOutType*>(taskData->inputs[i]);
            for (int j = 0; j < taskData->inputs_count[i]; j++) {
                input_[i][j] = tmp_ptr_[j];
            }
        }

        // Init value for output
        dor_product = 0;
        return true;
    }

    bool validation() override {
        // Check count elements of output
        if (taskData->outputs_count[0] == 1 &&
            taskData->inputs_count[0] == taskData->inputs_count[1]) {
            return true;
        } else {
            return false;
        }
    }

    bool run() override {
        dor_product = std::inner_product(input_[0].begin(), input_[0].end(), input_[1].begin(), 0.0);
        return true;
    }

    bool post_processing() override {
        reinterpret_cast<InOutType*>(taskData->outputs[0])[0] = dor_product;
        return true;
    }

 private:
    std::vector< std::vector<InOutType> > input_;
    InOutType dor_product;
};

}  // namespace reference
}  // namespace ppc

#endif  // MODULES_REFERENCE_VECTOR_DOT_PRODUCT_REF_TASK_HPP_
