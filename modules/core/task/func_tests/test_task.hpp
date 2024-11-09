#ifndef MODULES_CORE_TESTS_TEST_TASK_HPP_
#define MODULES_CORE_TESTS_TEST_TASK_HPP_

#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <numeric>

#include "core/task/include/task.hpp"

namespace ppc::test::task {

template <class T>
class TestTask : public ppc::core::Task<T, T> {
 public:
  explicit TestTask(TestTask::TaskData taskData_) : TestTask::Task(taskData_) {}
  bool pre_processing_impl() override {
    input_ = this->taskData.input;
    output_ = this->taskData.output.data();
    *output_ = 0;
    return true;
  }

 protected:
  bool validation_impl() override { return this->taskData.output.size() == 1; }

  bool run_impl() override {
    *output_ = std::accumulate(input_.begin(), input_.end(), T{});
    return true;
  }

  bool post_processing_impl() override { return true; }

 private:
  std::span<T> input_{};
  T *output_{};
};

}  // namespace ppc::test::task

#endif  // MODULES_CORE_TESTS_TEST_TASK_HPP_
