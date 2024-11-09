#ifndef MODULES_CORE_INCLUDE_TASK_HPP_
#define MODULES_CORE_INCLUDE_TASK_HPP_

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ppc::core {

template <typename InType, typename OutType>
struct GenericTaskData {
  const std::span<InType> input;
  std::span<OutType> output;

  GenericTaskData(const std::span<InType>& input_, std::span<OutType> output_)
      : input(input_), output(std::move(output_)) {}
  GenericTaskData(const InType& input_, OutType& output_)
      : input(std::addressof(input_), 1), output(std::addressof(output_), 1) {}
  GenericTaskData(const std::span<InType>& input_, OutType& output_)
      : input(input_), output(std::addressof(output_), 1) {}
  GenericTaskData(const InType& input_, std::span<OutType> output_)
      : input(std::addressof(input_), 1), output(std::move(output_)) {}
};

// Memory of inputs and outputs need to be initialized before create object of
// Task class
class BaseTask {
 public:
  // validation of data and validation of task attributes before running
  virtual bool validate();

  // pre-processing of input data
  virtual bool pre_process();

  // implementation of current task
  virtual bool run();

  // post-processing of output data
  virtual bool post_process();

  enum class TestingMode { Func, Perf } testing_mode = TestingMode::Func;

 protected:
  void internal_order_test(const std::string& str = __builtin_FUNCTION());

  // implementation of "validation" function
  virtual bool validation_impl() = 0;

  // implementation of "pre_processing" function
  virtual bool pre_processing_impl() = 0;

  // implementation of "run" function
  virtual bool run_impl() = 0;

  // implementation of "post_processing" function
  virtual bool post_processing_impl() = 0;

  std::vector<std::string> functions_order;
  const double max_test_time = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point;
};

template <typename InType, typename OutType>
class Task : public BaseTask {
 public:
  using TaskData = GenericTaskData<InType, OutType>;

  explicit Task(TaskData taskData_);

  // set input and output data
  void set_data(TaskData taskData_);

  // get input and output data
  const TaskData& get_data() const;

  // create a new input and output data instance
  static TaskData make_data() { return {}; }

 protected:
  TaskData taskData;
};

}  // namespace ppc::core

template <typename InType, typename OutType>
ppc::core::Task<InType, OutType>::Task(TaskData taskData_)
    : taskData(std::move(taskData_)) {}

template <typename InType, typename OutType>
void ppc::core::Task<InType, OutType>::set_data(typename ppc::core::Task<InType, OutType>::TaskData taskData_) {
  taskData = std::move(taskData_);
  testing_mode = TestingMode::Func;
  functions_order.clear();
}

template <typename InType, typename OutType>
const typename ppc::core::Task<InType, OutType>::TaskData& ppc::core::Task<InType, OutType>::get_data() const {
  return taskData;
}

#endif  // MODULES_CORE_INCLUDE_TASK_HPP_
