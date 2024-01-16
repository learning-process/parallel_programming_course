// Copyright 2023 Nesterov Alexander

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
  constexpr const static double MIN_TIME = 0.05;
};

class Perf {
 public:
  // Init performance analysis with initialized task and initialized data
  explicit Perf(std::shared_ptr<Task> task_);
  // Set task with initialized task and initialized data for performance
  // analysis c
  void set_task(std::shared_ptr<Task> task_);
  // Check performance of full task's pipeline:  pre_processing() ->
  // validation() -> run() -> post_processing()
  void pipeline_run(const std::shared_ptr<PerfAttr>& perfAttr,
                    const std::shared_ptr<ppc::core::PerfResults>& perfResults);
  // Check performance of task's run() function
  void task_run(const std::shared_ptr<PerfAttr>& perfAttr, const std::shared_ptr<ppc::core::PerfResults>& perfResults);
  // Pint results for automation checkers
  static void print_perf_statistic(const std::shared_ptr<PerfResults>& perfResults);

 private:
  std::shared_ptr<Task> task;
  static void common_run(const std::shared_ptr<PerfAttr>& perfAttr, const std::function<void()>& pipeline,
                         const std::shared_ptr<ppc::core::PerfResults>& perfResults);
};

}  // namespace core
}  // namespace ppc

#endif  // MODULES_CORE_INCLUDE_PERF_HPP_
