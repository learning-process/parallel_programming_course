#ifndef MODULES_CORE_INCLUDE_PERF_HPP_
#define MODULES_CORE_INCLUDE_PERF_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc {
namespace core {

struct PerfAttr {
  // count of task's running
  uint64_t num_running;
  std::function<double(void)> current_timer = [&] { return 0.0; };
};

struct PerfResults {
  // measurement of task's time (in seconds)
  double time_sec = 0.0;
  enum TypeOfRunning { PIPELINE, TASK_RUN, NONE } type_of_running = NONE;
  constexpr const static double MAX_TIME = 10.0;
};

class Perf {
 public:
  // Init performance analysis with initialized task and initialized data
  explicit Perf(const std::shared_ptr<Task>& task);
  // Set task with initialized task and initialized data for performance
  // analysis c
  void SetTask(const std::shared_ptr<Task>& task);
  // Check performance of full task's pipeline:  pre_processing() ->
  // validation() -> run() -> post_processing()
  void PipelineRun(const std::shared_ptr<PerfAttr>& perf_attr,
                   const std::shared_ptr<ppc::core::PerfResults>& perf_results);
  // Check performance of task's run() function
  void TaskRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::shared_ptr<ppc::core::PerfResults>& perf_results);
  // Pint results for automation checkers
  static void PrintPerfStatistic(const std::shared_ptr<PerfResults>& perf_results);

 private:
  std::shared_ptr<Task> task;
  static void CommonRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::function<void()>& pipeline,
                        const std::shared_ptr<ppc::core::PerfResults>& perf_results);
};

}  // namespace core
}  // namespace ppc

#endif  // MODULES_CORE_INCLUDE_PERF_HPP_
