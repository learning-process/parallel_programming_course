#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ppc::core {

struct TaskData {
  std::vector<uint8_t *> inputs;
  std::vector<std::uint32_t> inputs_count;
  std::vector<uint8_t *> outputs;
  std::vector<std::uint32_t> outputs_count;
  enum StateOfTesting : uint8_t { kFunc, kPerf } state_of_testing;
};

using TaskDataPtr = std::shared_ptr<ppc::core::TaskData>;

// Memory of inputs and outputs need to be initialized before create object of
// Task class
class Task {
 public:
  explicit Task(TaskDataPtr task_data);

  // set input and output data
  void SetData(TaskDataPtr task_data);

  // validation of data and validation of task attributes before running
  virtual bool Validation();

  // pre-processing of input data
  virtual bool PreProcessing();

  // realization of current task
  virtual bool Run();

  // post-processing of output data
  virtual bool PostProcessing();

  // get input and output data
  [[nodiscard]] TaskDataPtr GetData() const;

  virtual ~Task();

 protected:
  void InternalOrderTest(const std::string &str = __builtin_FUNCTION());
  TaskDataPtr task_data;

  // implementation of "validation" function
  virtual bool ValidationImpl() = 0;

  // implementation of "PreProcessing" function
  virtual bool PreProcessingImpl() = 0;

  // implementation of "run" function
  virtual bool RunImpl() = 0;

  // implementation of "post_processing" function
  virtual bool PostProcessingImpl() = 0;

 private:
  std::vector<std::string> functions_order_;
  std::vector<std::string> right_functions_order_ = {"Validation", "PreProcessing", "Run", "PostProcessing"};
  const double max_test_time_ = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point_;
};

}  // namespace ppc::core
