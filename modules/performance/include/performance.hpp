#pragma once

#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "task/include/task.hpp"

using namespace ppc::task;

namespace ppc::performance {

/**
 * @brief Default timer function used for performance measurement.
 * @return A fake time value (-1.0).
 */
inline double DefaultTimer() { return -1.0; }

/**
 * @brief Attributes used to configure performance measurement.
 */
struct PerfAttr {
  /**
   * @brief Number of repetitions of the task for averaging performance.
   */
  uint64_t num_running = 5;

  /**
   * @brief Timer function returning current time in seconds.
   * Default is a fake function returning -1.0.
   */
  std::function<double()> current_timer = DefaultTimer;
};

/**
 * @brief Stores the results of a performance test.
 */
struct PerfResults {
  /**
   * @brief Measured execution time in seconds (average over runs).
   */
  double time_sec = 0.0;

  /**
   * @brief Type of performance test performed.
   */
  enum TypeOfRunning : uint8_t {
    kPipeline,  ///< Full pipeline: Validation → PreProcessing → Run → PostProcessing
    kTaskRun,   ///< Only Run() function is measured
    kNone       ///< No performance test was executed
  } type_of_running = kNone;

  /**
   * @brief Maximum allowed execution time in seconds.
   */
  constexpr static double kMaxTime = 10.0;
};

/**
 * @brief Converts a TypeOfRunning enum value to its string representation.
 * @param type_of_running Enum value indicating which performance type was run.
 * @return String name corresponding to the enum.
 */
inline std::string GetStringParamName(PerfResults::TypeOfRunning type_of_running) {
  switch (type_of_running) {
    case PerfResults::kTaskRun:
      return "task_run";
    case PerfResults::kPipeline:
      return "pipeline";
    default:
      return "none";
  }
}

/**
 * @brief Measures performance of a given task using configured attributes.
 * @tparam InType Input type of the task.
 * @tparam OutType Output type of the task.
 */
template <typename InType, typename OutType>
class Perf {
 public:
  /**
   * @brief Constructs a performance tester for the given task.
   * @param task_ptr Shared pointer to a task object.
   */
  explicit Perf(const ::TaskPtr<InType, OutType>& task_ptr) : task_(task_ptr) {
    task_ptr->GetStateOfTesting() = ::StateOfTesting::kPerf;
  }

  /**
   * @brief Runs the full task pipeline and measures its performance.
   * The full pipeline includes: Validation → PreProcessing → Run → PostProcessing.
   * @param perf_attr Performance measurement configuration.
   */
  void PipelineRun(const PerfAttr& perf_attr) {
    perf_results_.type_of_running = PerfResults::kPipeline;
    CommonRun(perf_attr, RunFullPipeline<::Task<InType, OutType>>);
  }

  /**
   * @brief Measures only the Run() part of the task.
   * Pre- / Post-processing and validation are still invoked before and after.
   * @param perf_attr Performance measurement configuration.
   */
  void TaskRun(const PerfAttr& perf_attr) {
    perf_results_.type_of_running = PerfResults::kTaskRun;

    task_->Validation();
    task_->PreProcessing();
    CommonRun(perf_attr, RunOnly<::Task<InType, OutType>>);
    task_->PostProcessing();

    // Ensure correctness after a performance run
    task_->Validation();
    task_->PreProcessing();
    task_->Run();
    task_->PostProcessing();
  }

  /**
   * @brief Prints formatted performance results or throws if too slow.
   * Prints output in format: test_id:type:time_in_seconds
   * @param test_id Identifier for the current test (e.g., "omp_4_threads").
   * @throws std::runtime_error if execution time exceeds allowed maximum.
   */
  void PrintPerfStatistic(const std::string& test_id) const {
    const auto& type = perf_results_.type_of_running;
    const std::string type_name = GetStringParamName(type);

    if (type == PerfResults::kNone) {
      throw std::runtime_error("The type of performance check for the task was not selected.\n");
    }

    std::stringstream ss;
    double time_secs = perf_results_.time_sec;

    if (time_secs < PerfResults::kMaxTime) {
      ss << std::fixed << std::setprecision(10) << time_secs;
    } else {
      ss << std::fixed << std::setprecision(10) << -1.0;
      std::stringstream err;
      err << "Task execute time need to be: time < " << PerfResults::kMaxTime
          << " secs.\nOriginal time in secs: " << time_secs << '\n';
      std::cout << test_id << ":" << type_name << ":" << ss.str() << '\n';
      throw std::runtime_error(err.str());
    }

    std::cout << test_id << ":" << type_name << ":" << ss.str() << '\n';
  }

  /**
   * @brief Retrieves performance test results.
   * @return Struct containing latest performance data.
   */
  [[nodiscard]] PerfResults GetPerfResults() const { return perf_results_; }

 private:
  PerfResults perf_results_;                       ///< Stores measurement results and mode.
  std::shared_ptr<::Task<InType, OutType>> task_;  ///< Pointer to task being tested.

  /**
   * @brief Executes the full pipeline for the given task.
   * @tparam TaskType Type of the task.
   * @param task Shared pointer to the task instance.
   */
  template <typename TaskType>
  static void RunFullPipeline(const std::shared_ptr<TaskType>& task) {
    task->Validation();
    task->PreProcessing();
    task->Run();
    task->PostProcessing();
  }

  /**
   * @brief Executes only the Run() method of the given task.
   * @tparam TaskType Type of the task.
   * @param task Shared pointer to the task instance.
   */
  template <typename TaskType>
  static void RunOnly(const std::shared_ptr<TaskType>& task) {
    task->Run();
  }

  /**
   * @brief Measures execution time of a given function over multiple runs.
   * @tparam Func Type of callable taking shared_ptr to task.
   * @param perf_attr Attributes controlling the number of runs and timer.
   * @param func Callable that invokes the desired part of the task.
   */
  template <typename Func>
  void CommonRun(const PerfAttr& perf_attr, Func func) {
    double begin = perf_attr.current_timer();
    for (uint64_t i = 0; i < perf_attr.num_running; ++i) {
      func(task_);
    }
    double end = perf_attr.current_timer();
    perf_results_.time_sec = (end - begin) / static_cast<double>(perf_attr.num_running);
  }
};

}  // namespace ppc::performance
