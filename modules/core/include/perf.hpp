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
    // Check performance of test function
    void pipeline_run(const std::function<void()>& test_function,
                      std::shared_ptr<PerfAttr> perfAttr,
                      std::shared_ptr<ppc::core::PerfResults> perfResults);
};

}  // namespace core
}  // namespace ppc

#endif  // MODULES_CORE_INCLUDE_PERF_HPP_
