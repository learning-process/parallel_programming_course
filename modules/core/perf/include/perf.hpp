#pragma once

#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/task/include/task.hpp"

namespace ppc::core {

struct PerfAttr {
  // count of task's running
  uint64_t num_running = 5;
  std::function<double()> current_timer = [&] { return -1.0; };
};

struct PerfResults {
  // measurement of task's time (in seconds)
  double time_sec = 0.0;
  enum TypeOfRunning : uint8_t { kPipeline, kTaskRun, kNone } type_of_running = kNone;
  constexpr static double kMaxTime = 10.0;
};

template <typename InType, typename OutType>
class Perf {
 public:
  // Init performance analysis with an initialized task and initialized data
  explicit Perf(const TaskPtr<InType, OutType>& task_ptr) : task_(task_ptr) {
    task_ptr->GetStateOfTesting() = StateOfTesting::kPerf;
  }
  // Check performance of full task's pipeline:  PreProcessing() ->
  // Validation() -> Run() -> PostProcessing()
  void PipelineRun(const PerfAttr& perf_attr) {
    perf_results_.type_of_running = PerfResults::TypeOfRunning::kPipeline;

    CommonRun(
        perf_attr,
        [&]() {
          task_->Validation();
          task_->PreProcessing();
          task_->Run();
          task_->PostProcessing();
        },
        perf_results_);
  }
  // Check performance of task's Run() function
  void TaskRun(const PerfAttr& perf_attr) {
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
  // Pint results for automation checkers
  void PrintPerfStatistic(const std::string& test_id) const {
    std::string type_test_name;
    if (perf_results_.type_of_running == PerfResults::TypeOfRunning::kTaskRun) {
      type_test_name = "task_run";
    } else if (perf_results_.type_of_running == PerfResults::TypeOfRunning::kPipeline) {
      type_test_name = "pipeline";
    } else {
      std::stringstream err_msg;
      err_msg << '\n' << "The type of performance check for the task was not selected.\n";
      throw std::runtime_error(err_msg.str().c_str());
    }

    auto time_secs = perf_results_.time_sec;
    std::stringstream perf_res_str;
    if (time_secs < PerfResults::kMaxTime) {
      perf_res_str << std::fixed << std::setprecision(10) << time_secs;
      std::cout << test_id << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
    } else {
      std::stringstream err_msg;
      err_msg << '\n' << "Task execute time need to be: ";
      err_msg << "time < " << PerfResults::kMaxTime << " secs." << '\n';
      err_msg << "Original time in secs: " << time_secs << '\n';
      perf_res_str << std::fixed << std::setprecision(10) << -1.0;
      std::cout << test_id << ":" << type_test_name << ":" << perf_res_str.str() << '\n';
      throw std::runtime_error(err_msg.str().c_str());
    }
  }
  // Get performance result structure of the current task
  PerfResults GetPerfResults() { return perf_results_; }

 private:
  PerfResults perf_results_;
  std::shared_ptr<Task<InType, OutType>> task_;
  static void CommonRun(const PerfAttr& perf_attr, const std::function<void()>& pipeline, PerfResults& perf_results) {
    auto begin = perf_attr.current_timer();
    for (uint64_t i = 0; i < perf_attr.num_running; i++) {
      pipeline();
    }
    auto end = perf_attr.current_timer();
    perf_results.time_sec = (end - begin) / perf_attr.num_running;
  }
};

inline std::string GetStringParamName(ppc::core::PerfResults::TypeOfRunning type_of_running) {
  if (type_of_running == core::PerfResults::kTaskRun) {
    return "task_run";
  }
  if (type_of_running == core::PerfResults::kPipeline) {
    return "pipeline";
  }
  return "none";
}

}  // namespace ppc::core
