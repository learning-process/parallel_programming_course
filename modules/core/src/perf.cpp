// Copyright 2023 Nesterov Alexander
#include <utility>
#include <iostream>
#include "core/include/perf.hpp"
#include "unapproved/unapproved.h"

void ppc::core::Perf::pipeline_run(const std::function<void()>& test_function, std::shared_ptr<PerfAttr> perfAttr,
                                   std::shared_ptr<ppc::core::PerfResults> perfResults) {
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < perfAttr->num_running; i++) {
        test_function();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    perfResults->time_sec = duration * 1e-9;
}
