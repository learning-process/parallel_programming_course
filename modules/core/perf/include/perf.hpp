#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "core/task/include/task.hpp"

namespace ppc::core {

struct PerfAttr {
  // count of task's running
  uint64_t num_running;
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
  // Set task with initialized task and initialized data for performance
  // analysis c
  void SetTask(const std::shared_ptr<Task>& task_ptr);
  // Check performance of full task's pipeline:  PreProcessing() ->
  // Validation() -> Run() -> PostProcessing()
  void PipelineRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::shared_ptr<PerfResults>& perf_results) const;
  // Check performance of task's Run() function
  void TaskRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::shared_ptr<PerfResults>& perf_results) const;
  // Pint results for automation checkers
  static void PrintPerfStatistic(const std::shared_ptr<PerfResults>& perf_results);

 private:
  std::shared_ptr<Task> task_;
  static void CommonRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::function<void()>& pipeline,
                        const std::shared_ptr<PerfResults>& perf_results);
};

}  // namespace ppc::core
