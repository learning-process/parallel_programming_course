#ifndef MODULES_CORE_INCLUDE_PERF_HPP_
#define MODULES_CORE_INCLUDE_PERF_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::core {

struct PerfAttr {
  // count of task's running
  size_t num_running;
  std::function<double(void)> current_timer = [&] { return 0.0; };
};

struct PerfResults {
  // measurement of task's time (in seconds)
  double time_sec = 0.0;
  enum class RunType : std::uint8_t { Pipeline, TaskRun, None } run_type = RunType::None;

  constexpr const static double MAX_TIME = 10.0;
};

class Perf {
 public:
  // Init performance analysis with initialized task and initialized data
  explicit Perf(BaseTask& task_);
  // Set task with initialized task and initialized data for performance
  // analysis c
  void set_task(BaseTask& task_);
  // Check performance of full task's pipeline:  pre_processing() ->
  // validation() -> run() -> post_processing()
  void pipeline_run(PerfAttr& perfAttr, ppc::core::PerfResults& perfResults);
  // Check performance of task's run() function
  void task_run(PerfAttr& perfAttr, ppc::core::PerfResults& perfResults);
  // Pint results for automation checkers
  static void print_perf_statistic(const PerfResults& perfResults);

 private:
  BaseTask* task{nullptr};
  static void common_run(PerfAttr& perfAttr, const std::function<void()>& pipeline,
                         ppc::core::PerfResults& perfResults);
};

}  // namespace ppc::core

#endif  // MODULES_CORE_INCLUDE_PERF_HPP_
