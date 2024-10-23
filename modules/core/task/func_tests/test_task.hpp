#ifndef MODULES_CORE_TESTS_TEST_TASK_HPP_
#define MODULES_CORE_TESTS_TEST_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::test {

template <class T>
class TestTask : public ppc::core::Task {
 public:
  explicit TestTask(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
  bool pre_processing() override {
    internal_order_test();
    input_ = reinterpret_cast<T *>(taskData->inputs[0]);
    output_ = reinterpret_cast<T *>(taskData->outputs[0]);
    output_[0] = 0;
    return true;
  }

  bool validation() override {
    internal_order_test();
    return taskData->outputs_count[0] == 1;
  }

  bool run() override {
    internal_order_test();
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      output_[0] += input_[i];
    }
    return true;
  }

  bool post_processing() override {
    internal_order_test();
    return true;
  }

 private:
  T *input_{};
  T *output_{};
};

}  // namespace ppc::test

#endif  // MODULES_CORE_TESTS_TEST_TASK_HPP_
