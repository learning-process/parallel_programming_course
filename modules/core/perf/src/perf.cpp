// Copyright 2023 Nesterov Alexander
#include "core/perf/include/perf.hpp"

#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

ppc::core::Perf::Perf(std::shared_ptr<Task> task_) { set_task(std::move(task_)); }

void ppc::core::Perf::set_task(std::shared_ptr<Task> task_) {
  task_->get_data()->state_of_testing = TaskData::StateOfTesting::PERF;
  task = std::move(task_);
}

void ppc::core::Perf::pipeline_run(const std::shared_ptr<PerfAttr>& perfAttr,
                                   const std::shared_ptr<ppc::core::PerfResults>& perfResults) {
  perfResults->type_of_running = PerfResults::TypeOfRunning::PIPELINE;

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
  perfResults->type_of_running = PerfResults::TypeOfRunning::TASK_RUN;

  task->validation();
  task->pre_processing();
  common_run(std::move(perfAttr), [&]() { task->run(); }, std::move(perfResults));
  task->post_processing();

  task->validation();
  task->pre_processing();
  task->run();
  task->post_processing();
}

void ppc::core::Perf::common_run(const std::shared_ptr<PerfAttr>& perfAttr, const std::function<void()>& pipeline,
                                 const std::shared_ptr<ppc::core::PerfResults>& perfResults) {
  auto begin = perfAttr->current_timer();
  for (uint64_t i = 0; i < perfAttr->num_running; i++) {
    pipeline();
  }
  auto end = perfAttr->current_timer();
  perfResults->time_sec = end - begin;
}

void ppc::core::Perf::print_perf_statistic(const std::shared_ptr<PerfResults>& perfResults) {
  std::string relative_path(::testing::UnitTest::GetInstance()->current_test_info()->file());
  std::string ppc_regex_template("parallel_programming_course");
  std::string perf_regex_template("perf_tests");
  std::string type_test_name;

  auto time_secs = perfResults->time_sec;

  if (perfResults->type_of_running == PerfResults::TypeOfRunning::TASK_RUN) {
    type_test_name = "task_run";
  } else if (perfResults->type_of_running == PerfResults::TypeOfRunning::PIPELINE) {
    type_test_name = "pipeline";
  } else if (perfResults->type_of_running == PerfResults::TypeOfRunning::NONE) {
    type_test_name = "none";
  }

  auto first_found_position = relative_path.find(ppc_regex_template) + ppc_regex_template.length() + 1;
  relative_path.erase(0, first_found_position);

  auto last_found_position = relative_path.find(perf_regex_template) - 1;
  relative_path.erase(last_found_position, relative_path.length() - 1);

  std::stringstream perf_res_str;
  if (time_secs > PerfResults::MIN_TIME && time_secs < PerfResults::MAX_TIME) {
    perf_res_str << std::fixed << std::setprecision(10) << time_secs;
  } else {
    std::cerr << "Task execute time need to be: ";
    std::cerr << PerfResults::MIN_TIME << " secs. < time < " << PerfResults::MAX_TIME << " secs." << std::endl;
    std::cerr << "Original time in secs: " << time_secs;
    perf_res_str << std::fixed << std::setprecision(10) << -1.0;
    EXPECT_TRUE(time_secs > PerfResults::MIN_TIME && time_secs < PerfResults::MAX_TIME);
  }

  std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << std::endl;
}
