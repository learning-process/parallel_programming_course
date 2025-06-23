#pragma once

#include <algorithm>
#include <chrono>
#include <core/util/include/util.hpp>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ppc::core {

/// @brief Represents the type of task (parallelization technology).
/// @details Used to select the implementation type in tests and execution logic.
enum TypeOfTask : uint8_t {
  /// Use all available implementations
  kALL,
  /// MPI (Message Passing Interface)
  kMPI,
  /// OpenMP (Open Multi-Processing)
  kOMP,
  /// Sequential implementation
  kSEQ,
  /// Standard Thread Library (STL threads)
  kSTL,
  /// Intel Threading Building Blocks (TBB)
  kTBB,
  /// Unknown task type
  kUnknown
};

/// @brief Indicates whether a task is enabled or disabled.
enum StatusOfTask : uint8_t {
  /// Task is enabled and should be executed
  kEnabled,
  /// Task is disabled and will be skipped
  kDisabled
};

/// @brief Returns a string representation of the task status.
/// @param status_of_task Task status (enabled or disabled).
/// @return "enabled" if the task is enabled, otherwise "disabled".
inline std::string GetStringTaskStatus(StatusOfTask status_of_task) {
  if (status_of_task == kDisabled) {
    return "disabled";
  }
  return "enabled";
}

/// @brief Returns a string representation of the task type based on the JSON settings file.
/// @param type_of_task Type of the task.
/// @param settings_file_path Path to the JSON file containing task type strings.
/// @return Formatted string combining the task type and its corresponding value from the file.
/// @throws std::runtime_error If the file cannot be opened.
inline std::string GetStringTaskType(TypeOfTask type_of_task, const std::string &settings_file_path) {
  std::ifstream file(settings_file_path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open " + settings_file_path);
  }

  auto list_settings = ppc::util::InitJSONPtr();
  file >> *list_settings;

  auto to_type_str = [&](const std::string &type) -> std::string {
    return type + "_" + std::string((*list_settings)["tasks"][type]);
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

template <typename InType, typename OutType>
/// @brief Base abstract class representing a generic task with a defined pipeline.
/// @tparam InType Input data type.
/// @tparam OutType Output data type.
class Task {
 public:
  /// @brief Constructs a new Task object.
  explicit Task(StateOfTesting /*state_of_testing*/ = StateOfTesting::kFunc) { functions_order_.clear(); }

  /// @brief Validates input data and task attributes before execution.
  /// @return True if validation is successful.
  virtual bool Validation() final {
    InternalOrderTest(ppc::util::FuncName());
    return ValidationImpl();
  }

  /// @brief Performs preprocessing on the input data.
  /// @return True if preprocessing is successful.
  virtual bool PreProcessing() final {
    InternalOrderTest(ppc::util::FuncName());
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest(ppc::util::FuncName());
    }
    return PreProcessingImpl();
  }

  /// @brief Executes the main logic of the task.
  /// @return True if execution is successful.
  virtual bool Run() final {
    InternalOrderTest(ppc::util::FuncName());
    return RunImpl();
  }

  /// @brief Performs postprocessing on the output data.
  /// @return True if postprocessing is successful.
  virtual bool PostProcessing() final {
    InternalOrderTest(ppc::util::FuncName());
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest(ppc::util::FuncName());
    }
    return PostProcessingImpl();
  }

  /// @brief Returns the current testing mode.
  /// @return Reference to the current StateOfTesting.
  StateOfTesting &GetStateOfTesting() { return state_of_testing_; }

  /// @brief Sets the dynamic task type.
  /// @param type_of_task Task type to set.
  void SetTypeOfTask(TypeOfTask type_of_task) { type_of_task_ = type_of_task; }

  /// @brief Returns the dynamic task type.
  /// @return Current dynamic task type.
  [[nodiscard]] TypeOfTask GetDynamicTypeOfTask() const { return type_of_task_; }

  /// @brief Returns the current task status.
  /// @return Task status (enabled or disabled).
  [[nodiscard]] StatusOfTask GetStatusOfTask() const { return status_of_task_; }

  /// @brief Returns the static task type.
  /// @return Static task type (default: kUnknown).
  static constexpr TypeOfTask GetStaticTypeOfTask() { return TypeOfTask::kUnknown; }

  /// @brief Returns a reference to the input data.
  /// @return Reference to the task's input data.
  InType &GetInput() { return input_; }

  /// @brief Returns a reference to the output data.
  /// @return Reference to the task's output data.
  OutType &GetOutput() { return output_; }

  /// @brief Destructor. Verifies that the pipeline was executed in the correct order.
  /// @note Terminates the program if pipeline order is incorrect or incomplete.
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
  /// @brief Verifies the correct order of pipeline method calls.
  /// @param str Name of the method being called.
  virtual void InternalOrderTest(const std::string &str) final {
    functions_order_.push_back(str);
    if (str == "PostProcessing" && IsFullPipelineStage()) {
      functions_order_.clear();
    } else {
      was_worked_ = true;
    }
  }

  /// @brief Measures execution time between preprocessing and postprocessing steps.
  /// @param str Name of the method being timed.
  /// @throws std::runtime_error If execution exceeds the allowed time limit.
  virtual void InternalTimeTest(const std::string &str) final {
    if (str == "PreProcessing") {
      tmp_time_point_ = std::chrono::high_resolution_clock::now();
    }

    if (str == "PostProcessing") {
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() -
                                                                           tmp_time_point_)
                          .count();
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

  /// @brief User-defined validation logic.
  /// @return True if validation is successful.
  virtual bool ValidationImpl() = 0;

  /// @brief User-defined preprocessing logic.
  /// @return True if preprocessing is successful.
  virtual bool PreProcessingImpl() = 0;

  /// @brief User-defined task execution logic.
  /// @return True if run is successful.
  virtual bool RunImpl() = 0;

  /// @brief User-defined postprocessing logic.
  /// @return True if postprocessing is successful.
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
    if (functions_order_.size() < 4) {
      return false;
    }

    auto it = std::adjacent_find(functions_order_.begin() + 2,
                                 functions_order_.begin() + static_cast<long>(functions_order_.size() - 2),
                                 std::not_equal_to<>());

    return (functions_order_[0] == "Validation" && functions_order_[1] == "PreProcessing" &&
            functions_order_[2] == "Run" &&
            it == (functions_order_.begin() + static_cast<long>(functions_order_.size() - 2)) &&
            functions_order_[functions_order_.size() - 1] == "PostProcessing");
  }
};

/// @brief Smart pointer alias for Task.
/// @tparam InType Input data type.
/// @tparam OutType Output data type.
template <typename InType, typename OutType>
using TaskPtr = std::shared_ptr<Task<InType, OutType>>;

/// @brief Constructs and returns a shared pointer to a task with the given input.
/// @tparam TaskType Type of the task to create.
/// @tparam InType Type of the input.
/// @param in Input to pass to the task constructor.
/// @return Shared pointer to the newly created task.
template <typename TaskType, typename InType>
std::shared_ptr<TaskType> TaskGetter(InType in) {
  return std::make_shared<TaskType>(in);
}

}  // namespace ppc::core
