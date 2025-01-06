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

using TaskDataPtr = std::shared_ptr<ppc::core::TaskData>;

// Memory of inputs and outputs need to be initialized before create object of
// Task class
class Task {
 public:
  explicit Task(TaskDataPtr task_data);

  // set input and output data
  void set_data(TaskDataPtr task_data);

  // validation of data and validation of task attributes before running
  virtual bool validation();

  // pre-processing of input data
  virtual bool pre_processing();

  // realization of current task
  virtual bool run();

  // post-processing of output data
  virtual bool post_processing();

  // get input and output data
  [[nodiscard]] TaskDataPtr get_data() const;

  virtual ~Task();

 protected:
  void internal_order_test(const std::string &str = __builtin_FUNCTION());
  TaskDataPtr taskData;

  // implementation of "validation" function
  virtual bool validation_impl() = 0;

  // implementation of "pre_processing" function
  virtual bool pre_processing_impl() = 0;

  // implementation of "run" function
  virtual bool run_impl() = 0;

  // implementation of "post_processing" function
  virtual bool post_processing_impl() = 0;

 private:
  std::vector<std::string> functions_order;
  std::vector<std::string> right_functions_order = {"validation", "pre_processing", "run", "post_processing"};
  const double max_test_time = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point;
};

}  // namespace ppc::core

#endif  // MODULES_CORE_INCLUDE_TASK_HPP_
