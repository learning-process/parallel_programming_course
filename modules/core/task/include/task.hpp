#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ppc::core {

// Memory of inputs and outputs need to be initialized before create object of
// Task class
class Task {
 public:
  enum StateOfTesting : uint8_t { kFunc, kPerf };

  explicit Task(StateOfTesting state_of_testing = StateOfTesting::kFunc);

  // validation of data and validation of task attributes before running
  virtual bool Validation() final;

  // pre-processing of input data
  virtual bool PreProcessing() final;

  // realization of the current task
  virtual bool Run() final;

  // post-processing of output data
  virtual bool PostProcessing() final;

  // get state of testing
  StateOfTesting &GetStateOfTesting() { return state_of_testing_; }

  template<typename Class, typename T>
  static T Get(const Class& obj, T Class::* member) {
    return obj.*member;
  }

  virtual ~Task();

 protected:
  virtual void InternalOrderTest(const std::string &str) final;

  virtual void InternalTimeTest(const std::string &str) final;

  // implementation of "Validation" function
  virtual bool ValidationImpl() = 0;

  // implementation of "PreProcessing" function
  virtual bool PreProcessingImpl() = 0;

  // implementation of "Run" function
  virtual bool RunImpl() = 0;

  // implementation of "PostProcessing" function
  virtual bool PostProcessingImpl() = 0;

 private:
  StateOfTesting state_of_testing_;
  std::vector<std::string> functions_order_;
  std::vector<std::string> right_functions_order_ = {"Validation", "PreProcessing", "Run", "PostProcessing"};
  static constexpr double kMaxTestTime = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point_;
  bool was_worked_ = false;

  bool IsFullPipelineStage();
};

using TaskPtr = std::shared_ptr<ppc::core::Task>;

template <typename TaskType, typename OutputType>
std::shared_ptr<TaskType> task_getter(OutputType in) {
  return std::make_shared<TaskType>(in);
}

template <typename TaskType, typename OutputType>
OutputType data_getter(TaskPtr current_task) {
  auto inheritance_task = std::dynamic_pointer_cast<TaskType>(current_task);
  return ppc::core::Task::Get(*inheritance_task, &TaskType::output_);
}

}  // namespace ppc::core
