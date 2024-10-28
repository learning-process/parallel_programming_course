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
  explicit TestTask(ppc::core::TaskDataPtr taskData_) : Task(taskData_) {}

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
    return true;
  }

  bool post_processing_impl() override { return true; }

 private:
  T *input_{};
  T *output_{};
};

template <class T>
class FakePerfTask : public TestTask<T> {
 public:
  explicit FakePerfTask(ppc::core::TaskDataPtr perfTaskData_) : TestTask<T>(perfTaskData_) {}

  bool run_impl() override {
    std::this_thread::sleep_for(20000ms);
    return TestTask<T>::run_impl();
  }
};

}  // namespace ppc::test::perf

#endif  // MODULES_CORE_TESTS_TEST_TASK_HPP_
