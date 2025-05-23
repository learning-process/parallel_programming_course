#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "core/task/include/task.hpp"

using namespace std::chrono;

namespace ppc::core {

// Memory of inputs and outputs need to be initialized before create object of
// Task class
template <typename InType, typename OutType>
class Task {
 public:
  enum StateOfTesting : uint8_t { kFunc, kPerf };

  explicit Task(StateOfTesting /*state_of_testing*/ = StateOfTesting::kFunc) {
    auto custom_terminate = []() {
      std::cerr << "ORDER OF FUNCTIONS IS NOT RIGHT! \n"
                   "Expected - \"Validation\", \"PreProcessing\", \"Run\", \"PostProcessing\" \n";
      std::exit(404);
    };
    std::set_terminate(custom_terminate);
    functions_order_.clear();
  }

  // validation of data and validation of task attributes before running
  virtual bool Validation() final {
    InternalOrderTest(__builtin_FUNCTION());
    return ValidationImpl();
  }

  // pre-processing of input data
  virtual bool PreProcessing() final {
    InternalOrderTest(__builtin_FUNCTION());
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest(__builtin_FUNCTION());
    }
    return PreProcessingImpl();
  }

  // realization of the current task
  virtual bool Run() final {
    InternalOrderTest(__builtin_FUNCTION());
    return RunImpl();
  }

  // post-processing of output data
  virtual bool PostProcessing() final {
    InternalOrderTest(__builtin_FUNCTION());
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest(__builtin_FUNCTION());
    }
    return PostProcessingImpl();
  }

  // get state of testing
  StateOfTesting &GetStateOfTesting() { return state_of_testing_; }

  InType &GetInput() { return input_; }

  OutType &GetOutput() { return output_; }

  virtual ~Task() {
    if (!functions_order_.empty() || !was_worked_) {
      std::terminate();
    } else {
      functions_order_.clear();
    }
  }

 protected:
  virtual void InternalOrderTest(const std::string &str) final {
    functions_order_.push_back(str);
    if (str == "PostProcessing" && IsFullPipelineStage()) {
      functions_order_.clear();
    } else {
      was_worked_ = true;
    }
  }

  virtual void InternalTimeTest(const std::string &str) final {
    if (str == "PreProcessing") {
      tmp_time_point_ = std::chrono::high_resolution_clock::now();
    }

    if (str == "PostProcessing") {
      auto duration = duration_cast<nanoseconds>(high_resolution_clock::now() - tmp_time_point_).count();
      auto diff = static_cast<double>(duration) * 1e-9;

      std::stringstream err_msg;
      if (diff < kMaxTestTime) {
        err_msg << "Test time:" << std::fixed << std::setprecision(10) << diff << '\n';
      } else {
        err_msg << "\nTask execute time need to be: ";
        err_msg << "time < " << kMaxTestTime << " secs.\n";
        err_msg << "Original time in secs: " << diff << '\n';
        throw std::runtime_error(err_msg.str().c_str());
      }
    }
  }

  // implementation of "Validation" function
  virtual bool ValidationImpl() = 0;

  // implementation of "PreProcessing" function
  virtual bool PreProcessingImpl() = 0;

  // implementation of "Run" function
  virtual bool RunImpl() = 0;

  // implementation of "PostProcessing" function
  virtual bool PostProcessingImpl() = 0;

 private:
  InType input_;
  OutType output_;
  StateOfTesting state_of_testing_ = kFunc;
  std::vector<std::string> functions_order_;
  std::vector<std::string> right_functions_order_ = {"Validation", "PreProcessing", "Run", "PostProcessing"};
  static constexpr double kMaxTestTime = 1.0;
  std::chrono::high_resolution_clock::time_point tmp_time_point_;
  bool was_worked_ = false;

  bool IsFullPipelineStage() {
    auto it = std::adjacent_find(functions_order_.begin() + 2,
                                 functions_order_.begin() + static_cast<long>(functions_order_.size() - 2),
                                 std::not_equal_to<>());

    return (functions_order_.size() >= 4 && functions_order_[0] == "Validation" &&
            functions_order_[1] == "PreProcessing" && functions_order_[2] == "Run" &&
            it == (functions_order_.begin() + static_cast<long>(functions_order_.size() - 2)) &&
            functions_order_[functions_order_.size() - 1] == "PostProcessing");
  }
};

template <typename InType, typename OutType>
using TaskPtr = std::shared_ptr<ppc::core::Task<InType, OutType>>;

template <typename TaskType, typename InType>
std::shared_ptr<TaskType> TaskGetter(InType in) {
  return std::make_shared<TaskType>(in);
}

}  // namespace ppc::core
