#include "core/perf/include/perf.hpp"

#include <gtest/gtest.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

ppc::core::Perf::Perf(BaseTask& task_) { set_task(task_); }

void ppc::core::Perf::set_task(BaseTask& task_) {
  task_.testing_mode = BaseTask::TestingMode::Perf;
  task = &task_;
}

void ppc::core::Perf::pipeline_run(PerfAttr& perfAttr, ppc::core::PerfResults& perfResults) {
  perfResults.run_type = PerfResults::RunType::Pipeline;

  common_run(
      perfAttr,
      [&]() {
        task->validate();
        task->pre_process();
        task->run();
        task->post_process();
      },
      perfResults);
}

void ppc::core::Perf::task_run(PerfAttr& perfAttr, ppc::core::PerfResults& perfResults) {
  perfResults.run_type = PerfResults::RunType::TaskRun;

  task->validate();
  task->pre_process();
  common_run(
      perfAttr, [&]() { task->run(); }, perfResults);
  task->post_process();

  task->validate();
  task->pre_process();
  task->run();
  task->post_process();
}

void ppc::core::Perf::common_run(PerfAttr& perfAttr, const std::function<void()>& pipeline,
                                 ppc::core::PerfResults& perfResults) {
  const auto begin = perfAttr.current_timer();
  for (size_t i = 0; i < perfAttr.num_running; i++) {
    pipeline();
  }
  const auto end = perfAttr.current_timer();
  perfResults.time_sec = end - begin;
}

void ppc::core::Perf::print_perf_statistic(const PerfResults& perfResults) {
  std::string relative_path(::testing::UnitTest::GetInstance()->current_test_info()->file());
  std::string ppc_regex_template("parallel_programming_course");
  std::string perf_regex_template("perf_tests");
  std::string type_test_name;

  auto time_secs = perfResults.time_sec;

  switch (perfResults.run_type) {
    case PerfResults::RunType::TaskRun:
      type_test_name = "task_run";
      break;
    case PerfResults::RunType::Pipeline:
      type_test_name = "pipeline";
      break;
    case PerfResults::RunType::None:
      type_test_name = "none";
      break;
    default:
      assert(false);
      break;
  }

  const auto first_found_position = relative_path.find(ppc_regex_template) + ppc_regex_template.length() + 1;
  relative_path.erase(0, first_found_position);

  const auto last_found_position = relative_path.find(perf_regex_template) - 1;
  relative_path.erase(last_found_position, relative_path.length() - 1);

  std::stringstream perf_res_str;
  if (time_secs < PerfResults::MAX_TIME) {
    perf_res_str << std::fixed << std::setprecision(10) << time_secs;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << std::endl;
  } else {
    std::stringstream errMsg;
    errMsg << "\nTask execute time need to be: ";
    errMsg << "time < " << PerfResults::MAX_TIME << " secs.\n";
    errMsg << "Original time in secs: " << time_secs << '\n';
    perf_res_str << std::fixed << std::setprecision(10) << -1.0;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << std::endl;
    throw std::runtime_error(errMsg.str());
  }
}
