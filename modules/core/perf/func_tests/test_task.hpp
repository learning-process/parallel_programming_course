// Copyright 2023 Nesterov Alexander

#ifndef MODULES_CORE_TESTS_TEST_TASK_HPP_
#define MODULES_CORE_TESTS_TEST_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

using namespace std::chrono_literals;

namespace ppc::test {

template <class T>
class TestTask : public ppc::core::Task {
 public:
  explicit TestTask(std::shared_ptr<ppc::core::TaskData> taskData_, bool pause_ = false)
      : Task(taskData_), pause(pause_) {}
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
    if (pause) {
      std::this_thread::sleep_for(20ms);
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
  bool pause;
};

}  // namespace ppc::test

#endif  // MODULES_CORE_TESTS_TEST_TASK_HPP_
