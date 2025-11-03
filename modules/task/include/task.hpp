#pragma once

#include <omp.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <util/include/util.hpp>
#include <utility>

namespace ppc::task {

/// @brief Represents the type of task (parallelization technology).
/// @details Used to select the implementation type in tests and execution logic.
enum class TypeOfTask : uint8_t {
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

using TaskMapping = std::pair<TypeOfTask, std::string>;
using TaskMappingArray = std::array<TaskMapping, 6>;

const TaskMappingArray kTaskTypeMappings = {{{TypeOfTask::kALL, "all"},
                                             {TypeOfTask::kMPI, "mpi"},
                                             {TypeOfTask::kOMP, "omp"},
                                             {TypeOfTask::kSEQ, "seq"},
                                             {TypeOfTask::kSTL, "stl"},
                                             {TypeOfTask::kTBB, "tbb"}}};

inline std::string TypeOfTaskToString(TypeOfTask type) {
  for (const auto &[key, value] : kTaskTypeMappings) {
    if (key == type) {
      return value;
    }
  }
  return "unknown";
}

/// @brief Indicates whether a task is enabled or disabled.
enum class StatusOfTask : uint8_t {
  /// Task is enabled and should be executed
  kEnabled,
  /// Task is disabled and will be skipped
  kDisabled
};

/// @brief Returns a string representation of the task status.
/// @param status_of_task Task status (enabled or disabled).
/// @return "enabled" if the task is enabled, otherwise "disabled".
inline std::string GetStringTaskStatus(StatusOfTask status_of_task) {
  if (status_of_task == StatusOfTask::kDisabled) {
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

  std::string type_str = TypeOfTaskToString(type_of_task);
  if (type_str == "unknown") {
    return type_str;
  }

  return type_str + "_" + std::string((*list_settings)["tasks"][type_str]);
}

enum class StateOfTesting : uint8_t { kFunc, kPerf };

template <typename InType, typename OutType>
/// @brief Base abstract class representing a generic task with a defined pipeline.
/// @tparam InType Input data type.
/// @tparam OutType Output data type.
class Task {
 public:
  /// @brief Validates input data and task attributes before execution.
  /// @return True if validation is successful.
  virtual bool Validation() final {
    if (stage_ == PipelineStage::kNone || stage_ == PipelineStage::kDone) {
      stage_ = PipelineStage::kValidation;
    } else {
      stage_ = PipelineStage::kException;
      throw std::runtime_error("Validation should be called before preprocessing");
    }
    return ValidationImpl();
  }

  /// @brief Performs preprocessing on the input data.
  /// @return True if preprocessing is successful.
  virtual bool PreProcessing() final {
    if (stage_ == PipelineStage::kValidation) {
      stage_ = PipelineStage::kPreProcessing;
    } else {
      stage_ = PipelineStage::kException;
      throw std::runtime_error("Preprocessing should be called after validation");
    }
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest();
    }
    return PreProcessingImpl();
  }

  /// @brief Executes the main logic of the task.
  /// @return True if execution is successful.
  virtual bool Run() final {
    if (stage_ == PipelineStage::kPreProcessing || stage_ == PipelineStage::kRun) {
      stage_ = PipelineStage::kRun;
    } else {
      stage_ = PipelineStage::kException;
      throw std::runtime_error("Run should be called after preprocessing");
    }
    return RunImpl();
  }

  /// @brief Performs postprocessing on the output data.
  /// @return True if postprocessing is successful.
  virtual bool PostProcessing() final {
    if (stage_ == PipelineStage::kRun) {
      stage_ = PipelineStage::kDone;
    } else {
      stage_ = PipelineStage::kException;
      throw std::runtime_error("Postprocessing should be called after run");
    }
    if (state_of_testing_ == StateOfTesting::kFunc) {
      InternalTimeTest();
    }
    return PostProcessingImpl();
  }

  /// @brief Returns the current testing mode.
  /// @return Reference to the current StateOfTesting.
  StateOfTesting &GetStateOfTesting() {
    return state_of_testing_;
  }

  /// @brief Sets the dynamic task type.
  /// @param type_of_task Task type to set.
  void SetTypeOfTask(TypeOfTask type_of_task) {
    type_of_task_ = type_of_task;
  }

  /// @brief Returns the dynamic task type.
  /// @return Current dynamic task type.
  [[nodiscard]] TypeOfTask GetDynamicTypeOfTask() const {
    return type_of_task_;
  }

  /// @brief Returns the current task status.
  /// @return Task status (enabled or disabled).
  [[nodiscard]] StatusOfTask GetStatusOfTask() const {
    return status_of_task_;
  }

  /// @brief Returns the static task type.
  /// @return Static task type (default: kUnknown).
  static constexpr TypeOfTask GetStaticTypeOfTask() {
    return TypeOfTask::kUnknown;
  }

  /// @brief Returns a reference to the input data.
  /// @return Reference to the task's input data.
  InType &GetInput() {
    return input_;
  }

  /// @brief Returns a reference to the output data.
  /// @return Reference to the task's output data.
  OutType &GetOutput() {
    return output_;
  }

  /// @brief Destructor. Verifies that the pipeline was executed in the correct order.
  /// @note Terminates the program if the pipeline order is incorrect or incomplete.
  virtual ~Task() {
    if (stage_ != PipelineStage::kDone && stage_ != PipelineStage::kException) {
      ppc::util::DestructorFailureFlag::Set();
    }
#if _OPENMP >= 201811
    omp_pause_resource_all(omp_pause_soft);
#endif
  }

 protected:
  /// @brief Measures execution time between preprocessing and postprocessing steps.
  /// @throws std::runtime_error If execution exceeds the allowed time limit.
  virtual void InternalTimeTest() final {
    if (stage_ == PipelineStage::kPreProcessing) {
      tmp_time_point_ = std::chrono::high_resolution_clock::now();
    }

    if (stage_ == PipelineStage::kDone) {
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() -
                                                                           tmp_time_point_)
                          .count();
      auto diff = static_cast<double>(duration) * 1e-9;

      const auto max_time = ppc::util::GetTaskMaxTime();
      std::stringstream err_msg;
      if (diff < max_time) {
        err_msg << "Test time:" << std::fixed << std::setprecision(10) << diff << '\n';
      } else {
        err_msg << "\nTask execute time need to be: ";
        err_msg << "time < " << max_time << " secs.\n";
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
  /// @return True if a run is successful.
  virtual bool RunImpl() = 0;

  /// @brief User-defined postprocessing logic.
  /// @return True if postprocessing is successful.
  virtual bool PostProcessingImpl() = 0;

 private:
  InType input_{};
  OutType output_{};
  StateOfTesting state_of_testing_ = StateOfTesting::kFunc;
  TypeOfTask type_of_task_ = TypeOfTask::kUnknown;
  StatusOfTask status_of_task_ = StatusOfTask::kEnabled;
  std::chrono::high_resolution_clock::time_point tmp_time_point_;
  enum class PipelineStage : uint8_t {
    kNone,
    kValidation,
    kPreProcessing,
    kRun,
    kDone,
    kException
  } stage_ = PipelineStage::kNone;
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
/// @return Shared a pointer to the newly created task.
template <typename TaskType, typename InType>
std::shared_ptr<TaskType> TaskGetter(const InType &in) {
  return std::make_shared<TaskType>(in);
}

}  // namespace ppc::task
