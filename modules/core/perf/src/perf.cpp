// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include "core/perf/include/perf.hpp"

#include <chrono>
#include <iostream>
#include <utility>
#include <iomanip>
#include <sstream>

ppc::core::Perf::Perf(std::shared_ptr<Task> task_) { set_task(std::move(task_)); }

void ppc::core::Perf::set_task(std::shared_ptr<Task> task_) { task = std::move(task_); }

void ppc::core::Perf::pipeline_run(const std::shared_ptr<PerfAttr>& perfAttr,
                                   const std::shared_ptr<ppc::core::PerfResults>& perfResults) {
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

void ppc::core::Perf::task_run(const std::shared_ptr<PerfAttr>& perfAttr,
                               const std::shared_ptr<ppc::core::PerfResults>& perfResults) {
  task->validation();
  task->pre_processing();
  common_run(
      std::move(perfAttr), [&]() { task->run(); }, std::move(perfResults));
  task->post_processing();

  task->validation();
  task->pre_processing();
  task->run();
  task->post_processing();
}

void ppc::core::Perf::common_run(const std::shared_ptr<PerfAttr>& perfAttr, const std::function<void()>& pipeline,
                                 const std::shared_ptr<ppc::core::PerfResults>& perfResults) {
  auto begin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < perfAttr->num_running; i++) {
    pipeline();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
  perfResults->time_sec = static_cast<double>(duration) * 1e-9;
}

void ppc::core::Perf::print_perf_statistic(const std::shared_ptr<PerfResults>& perfResults) {
  std::cout << ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name() << "_";
  std::cout << ::testing::UnitTest::GetInstance()->current_test_info()->name() << ":";
  std::stringstream stream;
  stream << std::fixed << std::setprecision(10) << perfResults->time_sec;
  if (perfResults->time_sec > MAX_TIME) {
    std::cerr << "Task need to execute < " << MAX_TIME << " secs: " << perfResults->time_sec;
    exit(1);
  }
  std::cout << stream.str() << std::endl;
}
