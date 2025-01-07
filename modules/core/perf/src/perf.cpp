#include "core/perf/include/perf.hpp"

#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

ppc::core::Perf::Perf(const std::shared_ptr<Task>& task) { SetTask(task); }

void ppc::core::Perf::SetTask(const std::shared_ptr<Task>& task) {
  task->get_data()->state_of_testing = TaskData::StateOfTesting::PERF;
  this->task = task;
}

void ppc::core::Perf::PipelineRun(const std::shared_ptr<PerfAttr>& perf_attr,
                                  const std::shared_ptr<ppc::core::PerfResults>& perf_results) {
  perf_results->type_of_running = PerfResults::TypeOfRunning::PIPELINE;

  CommonRun(
      perf_attr,
      [&]() {
        task->validation();
        task->pre_processing();
        task->run();
        task->post_processing();
      },
      perf_results);
}

void ppc::core::Perf::TaskRun(const std::shared_ptr<PerfAttr>& perf_attr,
                              const std::shared_ptr<ppc::core::PerfResults>& perf_results) {
  perf_results->type_of_running = PerfResults::TypeOfRunning::TASK_RUN;

  task->validation();
  task->pre_processing();
  CommonRun(perf_attr, [&]() { task->run(); }, perf_results);
  task->post_processing();

  task->validation();
  task->pre_processing();
  task->run();
  task->post_processing();
}

void ppc::core::Perf::CommonRun(const std::shared_ptr<PerfAttr>& perf_attr, const std::function<void()>& pipeline,
                                const std::shared_ptr<ppc::core::PerfResults>& perf_results) {
  auto begin = perf_attr->current_timer();
  for (uint64_t i = 0; i < perf_attr->num_running; i++) {
    pipeline();
  }
  auto end = perf_attr->current_timer();
  perf_results->time_sec = end - begin;
}

void ppc::core::Perf::PrintPerfStatistic(const std::shared_ptr<PerfResults>& perf_results) {
  std::string relative_path(::testing::UnitTest::GetInstance()->current_test_info()->file());
  std::string ppc_regex_template("parallel_programming_course");
  std::string perf_regex_template("perf_tests");
  std::string type_test_name;

  auto time_secs = perf_results->time_sec;

  if (perf_results->type_of_running == PerfResults::TypeOfRunning::TASK_RUN) {
    type_test_name = "task_run";
  } else if (perf_results->type_of_running == PerfResults::TypeOfRunning::PIPELINE) {
    type_test_name = "pipeline";
  } else if (perf_results->type_of_running == PerfResults::TypeOfRunning::NONE) {
    type_test_name = "none";
  }

  auto first_found_position = relative_path.find(ppc_regex_template) + ppc_regex_template.length() + 1;
  relative_path.erase(0, first_found_position);

  auto last_found_position = relative_path.find(perf_regex_template) - 1;
  relative_path.erase(last_found_position, relative_path.length() - 1);

  std::stringstream perf_res_str;
  if (time_secs < PerfResults::MAX_TIME) {
    perf_res_str << std::fixed << std::setprecision(10) << time_secs;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
  } else {
    std::stringstream err_msg;
    err_msg << '\n' << "Task execute time need to be: ";
    err_msg << "time < " << PerfResults::MAX_TIME << " secs." << '\n';
    err_msg << "Original time in secs: " << time_secs << '\n';
    perf_res_str << std::fixed << std::setprecision(10) << -1.0;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
    throw std::runtime_error(err_msg.str().c_str());
  }
}
