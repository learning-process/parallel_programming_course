#include "core/perf/include/perf.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/task/include/task.hpp"

ppc::core::Perf::Perf(const std::shared_ptr<Task>& task_ptr) : task_(task_ptr) {
  task_ptr->GetStateOfTesting() = Task::StateOfTesting::kPerf;
}

void ppc::core::Perf::PipelineRun(const PerfAttr& perf_attr) {
  perf_results_.type_of_running = PerfResults::TypeOfRunning::kPipeline;

  CommonRun(perf_attr, [&]() {
    task_->Validation();
    task_->PreProcessing();
    task_->Run();
    task_->PostProcessing();
  }, perf_results_);
}

void ppc::core::Perf::TaskRun(const PerfAttr& perf_attr) {
  perf_results_.type_of_running = PerfResults::TypeOfRunning::kTaskRun;

  task_->Validation();
  task_->PreProcessing();
  CommonRun(perf_attr, [&]() { task_->Run(); }, perf_results_);
  task_->PostProcessing();

  task_->Validation();
  task_->PreProcessing();
  task_->Run();
  task_->PostProcessing();
}

void ppc::core::Perf::CommonRun(const PerfAttr& perf_attr, const std::function<void()>& pipeline,
                                ppc::core::PerfResults& perf_results) {
  auto begin = perf_attr.current_timer();
  for (uint64_t i = 0; i < perf_attr.num_running; i++) {
    pipeline();
  }
  auto end = perf_attr.current_timer();
  perf_results.time_sec = end - begin;
}

void ppc::core::Perf::PrintPerfStatistic() const {
  std::string relative_path(::testing::UnitTest::GetInstance()->current_test_info()->file());
  std::string ppc_regex_template("parallel_programming_course");
  std::string perf_regex_template("perf_tests");
  std::string type_test_name;

  auto time_secs = perf_results_.time_sec;

  if (perf_results_.type_of_running == PerfResults::TypeOfRunning::kTaskRun) {
    type_test_name = "task_run";
  } else if (perf_results_.type_of_running == PerfResults::TypeOfRunning::kPipeline) {
    type_test_name = "pipeline";
  } else {
    std::stringstream err_msg;
    err_msg << '\n' << "The type of performance check for the task was not selected.\n";
    throw std::runtime_error(err_msg.str().c_str());
  }

  auto first_found_position = relative_path.find(ppc_regex_template) + ppc_regex_template.length() + 1;
  relative_path.erase(0, first_found_position);

  auto last_found_position = relative_path.find(perf_regex_template) - 1;
  relative_path.erase(last_found_position, relative_path.length() - 1);

  std::stringstream perf_res_str;
  if (time_secs < PerfResults::kMaxTime) {
    perf_res_str << std::fixed << std::setprecision(10) << time_secs;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
  } else {
    std::stringstream err_msg;
    err_msg << '\n' << "Task execute time need to be: ";
    err_msg << "time < " << PerfResults::kMaxTime << " secs." << '\n';
    err_msg << "Original time in secs: " << time_secs << '\n';
    perf_res_str << std::fixed << std::setprecision(10) << -1.0;
    std::cout << relative_path << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
    throw std::runtime_error(err_msg.str().c_str());
  }
}

ppc::core::PerfResults ppc::core::Perf::GetPerfResults() { return perf_results_; }
