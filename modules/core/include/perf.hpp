// Copyright 2023 Nesterov Alexander

#ifndef MODULES_CORE_INCLUDE_PERF_HPP_
#define MODULES_CORE_INCLUDE_PERF_HPP_

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include "core/include/task.hpp"

namespace ppc {
namespace core {

struct PerfAttr {
    // count of task's running
    uint64_t num_running;
};

struct PerfResults {
    // measurement of task's time (in seconds)
    long double time_sec;
};

class Perf {
 public:
    // Init performance analysis with initialized task and initialized data
    explicit Perf(std::shared_ptr<Task> task_);
    // Set task with initialized task and initialized data for performance analysis c
    void set_task(std::shared_ptr<Task> task_);
    // Check performance of full task's pipeline:  pre_processing() -> validation() -> run() -> post_processing()
    void pipeline_run(std::shared_ptr<PerfAttr> perfAttr, std::shared_ptr<ppc::core::PerfResults> perfResults);
    // Check performance of task's run() function
    void task_run(std::shared_ptr<PerfAttr> perfAttr, std::shared_ptr<ppc::core::PerfResults> perfResults);
 private:
    std::shared_ptr<Task> task;
    static void common_run(std::shared_ptr<PerfAttr> perfAttr, std::function<void()> pipeline,
                           std::shared_ptr<ppc::core::PerfResults> perfResults);
};

}  // namespace core
}  // namespace ppc

#endif  // MODULES_CORE_INCLUDE_PERF_HPP_