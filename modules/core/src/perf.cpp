// Copyright 2023 Nesterov Alexander
#include "core/include/perf.hpp"

#include <iostream>
#include <utility>

#include "unapproved/unapproved.h"

ppc::core::Perf::Perf(std::shared_ptr<Task> task_) { set_task(std::move(task_)); }

void ppc::core::Perf::set_task(std::shared_ptr<Task> task_) { task = std::move(task_); }

void ppc::core::Perf::pipeline_run(std::shared_ptr<PerfAttr> perfAttr,
                                   std::shared_ptr<ppc::core::PerfResults> perfResults) {
  common_run(
      std::move(perfAttr),
      [&]() {
        task->validation();
        task->pre_processing();
        task->run();
        task->post_processing();
      },
      std::move(perfResults));
}

void ppc::core::Perf::task_run(std::shared_ptr<PerfAttr> perfAttr,
                               std::shared_ptr<ppc::core::PerfResults> perfResults) {
  task->validation();
  task->pre_processing();
  common_run(
      std::move(perfAttr), [&]() { task->run(); }, std::move(perfResults));
  task->post_processing();
}

void ppc::core::Perf::common_run(std::shared_ptr<PerfAttr> perfAttr, std::function<void()> pipeline,
                                 std::shared_ptr<ppc::core::PerfResults> perfResults) {
  auto begin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < perfAttr->num_running; i++) {
    pipeline();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
  perfResults->time_sec = duration * 1e-9;
}
