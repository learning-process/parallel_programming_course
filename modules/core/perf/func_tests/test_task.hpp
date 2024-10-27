#ifndef MODULES_CORE_TESTS_TEST_TASK_HPP_
#define MODULES_CORE_TESTS_TEST_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <thread>
#include <vector>

#include "core/task/include/task.hpp"

using namespace std::chrono_literals;

namespace ppc::test::perf {

template <class T>
class TestTask : public ppc::core::Task {
 public:
  explicit TestTask(std::shared_ptr<ppc::core::TaskData> taskData_, bool enableSleep_ = false)
      : Task(taskData_), enableSleep(enableSleep_) {}
  bool pre_processing_impl() override {
    input_ = reinterpret_cast<T *>(taskData->inputs[0]);
    output_ = reinterpret_cast<T *>(taskData->outputs[0]);
    output_[0] = 0;
    return true;
  }

  bool validation_impl() override { return taskData->outputs_count[0] == 1; }

  bool run_impl() override {
    for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
      output_[0] += input_[i];
    }
    if (enableSleep) {
      std::this_thread::sleep_for(20000ms);
    }
    return true;
  }

  bool post_processing_impl() override { return true; }

 private:
  T *input_{};
  T *output_{};
  bool enableSleep = false;
};

}  // namespace ppc::test::perf

#endif  // MODULES_CORE_TESTS_TEST_TASK_HPP_
