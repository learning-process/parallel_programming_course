#pragma once

#include <algorithm>
#include <chrono>
#include <core/util/include/util.hpp>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std::chrono;

namespace ppc::core {

enum TypeOfTask : uint8_t { kALL, kMPI, kOMP, kSEQ, kSTL, kTBB, kUnknown };
enum StatusOfTask : uint8_t { kEnabled, kDisabled };

inline std::string GetStringTaskStatus(StatusOfTask status_of_task) {
  if (status_of_task == kDisabled) {
    return "disabled";
  } else {
    return "enabled";
  }
}

inline std::string GetStringTaskType(TypeOfTask type_of_task, std::string settings_file_path) {
  std::ifstream file(settings_file_path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file settings.json");
  }
  nlohmann::json list_settings;
  file >> list_settings;

  auto to_type_str = [&](std::string type) -> std::string {
    return type + "_" + std::string(list_settings["tasks"][type]);
  };

  if (type_of_task == TypeOfTask::kALL) {
    return to_type_str("all");
  }
  if (type_of_task == TypeOfTask::kSTL) {
    return to_type_str("stl");
  }
  if (type_of_task == TypeOfTask::kOMP) {
    return to_type_str("omp");
  }
  if (type_of_task == TypeOfTask::kMPI) {
    return to_type_str("mpi");
  }
  if (type_of_task == TypeOfTask::kTBB) {
    return to_type_str("tbb");
  }
  if (type_of_task == TypeOfTask::kSEQ) {
    return to_type_str("seq");
  }
  return "unknown";
}

enum StateOfTesting : uint8_t { kFunc, kPerf };

// Memory of inputs and outputs need to be initialized before create an object of
// Task class
template <typename InType, typename OutType>
class Task {
 public:
  explicit Task(StateOfTesting /*state_of_testing*/ = StateOfTesting::kFunc) { functions_order_.clear(); }

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

  // set a type of task
  void SetTypeOfTask(TypeOfTask type_of_task) { type_of_task_ = type_of_task; }

  // get a dynamic type of task
  TypeOfTask GetDynamicTypeOfTask() { return type_of_task_; }

  // get a dynamic type of task
  StatusOfTask GetStatusOfTask() { return status_of_task_; }

  // get a static type of task
  static constexpr TypeOfTask GetStaticTypeOfTask() { return TypeOfTask::kUnknown; }

  InType &GetInput() { return input_; }

  OutType &GetOutput() { return output_; }

  virtual ~Task() {
    if (!functions_order_.empty() || !was_worked_) {
      std::cerr << "ORDER OF FUNCTIONS IS NOT RIGHT! \n Expected - \"Validation\", \"PreProcessing\", \"Run\", "
                   "\"PostProcessing\" \n";
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
  TypeOfTask type_of_task_ = kUnknown;
  StatusOfTask status_of_task_ = kEnabled;
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
using TaskPtr = std::shared_ptr<Task<InType, OutType>>;

template <typename TaskType, typename InType>
std::shared_ptr<TaskType> TaskGetter(InType in) {
  return std::make_shared<TaskType>(in);
}

}  // namespace ppc::core
