#pragma once

#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::test::task {

template <class T>
class TestTask : public ppc::core::Task {
 public:
  explicit TestTask(const ppc::core::TaskDataPtr &task_data) : Task(task_data) {}
  bool PreProcessingImpl() override {
    input_ = reinterpret_cast<T *>(task_data->inputs[0]);
    output_ = reinterpret_cast<T *>(task_data->outputs[0]);
    output_[0] = 0;
    return true;
  }

  bool ValidationImpl() override { return task_data->outputs_count[0] == 1; }

  bool RunImpl() override {
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      output_[0] += input_[i];
    }
    return true;
  }

  bool PostProcessingImpl() override { return true; }

 private:
  T *input_{};
  T *output_{};
};

}  // namespace ppc::test::task
