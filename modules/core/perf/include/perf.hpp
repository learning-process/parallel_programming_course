#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "core/task/include/task.hpp"

namespace ppc::core {

struct PerfAttr {
  // count of task's running
  uint64_t num_running = 10;
  std::function<double()> current_timer = [&] { return 0.0; };
};

struct PerfResults {
  // measurement of task's time (in seconds)
  double time_sec = 0.0;
  enum TypeOfRunning : uint8_t { kPipeline, kTaskRun, kNone } type_of_running = kNone;
  constexpr static double kMaxTime = 10.0;
};

class Perf {
 public:
  // Init performance analysis with initialized task and initialized data
  explicit Perf(const std::shared_ptr<Task>& task_ptr);
  // Check performance of full task's pipeline:  PreProcessing() ->
  // Validation() -> Run() -> PostProcessing()
  void PipelineRun(const PerfAttr& perf_attr);
  // Check performance of task's Run() function
  void TaskRun(const PerfAttr& perf_attr);
  // Pint results for automation checkers
  void PrintPerfStatistic() const;
  // Get performance result structure of the current task
  PerfResults GetPerfResults();

 private:
  PerfResults perf_results_;
  std::shared_ptr<Task> task_;
  static void CommonRun(const PerfAttr& perf_attr, const std::function<void()>& pipeline, PerfResults& perf_results);
};

}  // namespace ppc::core
