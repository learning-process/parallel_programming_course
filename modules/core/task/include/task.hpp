// Copyright 2023 Nesterov Alexander

#ifndef MODULES_CORE_INCLUDE_TASK_HPP_
#define MODULES_CORE_INCLUDE_TASK_HPP_

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ppc::core {

struct TaskData {
  std::vector<uint8_t *> inputs;
  std::vector<std::uint32_t> inputs_count;
  std::vector<uint8_t *> outputs;
  std::vector<std::uint32_t> outputs_count;
  enum StateOfTesting { FUNC, PERF } state_of_testing;
};

// Memory of inputs and outputs need to be initialized before create object of
// Task class
class Task {
 public:
  explicit Task(std::shared_ptr<TaskData> taskData_);

  // set input and output data
  void set_data(std::shared_ptr<TaskData> taskData_);

  // validation of data and validation of task attributes before running
  virtual bool validation() = 0;

  // pre-processing of input data
  virtual bool pre_processing() = 0;

  // realization of current task
  virtual bool run() = 0;

  // post-processing of output data
  virtual bool post_processing() = 0;

  // get input and output data
  [[nodiscard]] std::shared_ptr<TaskData> get_data() const;

  virtual ~Task();

 protected:
  void internal_order_test(const std::string &str = __builtin_FUNCTION());
  std::shared_ptr<TaskData> taskData;

 private:
  std::vector<std::string> functions_order;
  std::vector<std::string> right_functions_order = {"validation", "pre_processing", "run", "post_processing"};
  const double max_test_time = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point;
};

}  // namespace ppc::core

#endif  // MODULES_CORE_INCLUDE_TASK_HPP_
