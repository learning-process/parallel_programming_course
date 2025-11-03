#pragma once

#include <gtest/gtest.h>
#include <omp.h>

#include <chrono>
#include <csignal>
#include <cstddef>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "performance/include/performance.hpp"
#include "task/include/task.hpp"
#include "util/include/util.hpp"

namespace ppc::util {

double GetTimeMPI();
int GetMPIRank();

template <typename InType, typename OutType>
using PerfTestParam = std::tuple<std::function<ppc::task::TaskPtr<InType, OutType>(InType)>, std::string,
                                 ppc::performance::PerfResults::TypeOfRunning>;

template <typename InType, typename OutType>
/// @brief Base class for performance testing of parallel tasks.
/// @tparam InType Input data type.
/// @tparam OutType Output data type.
class BaseRunPerfTests : public ::testing::TestWithParam<PerfTestParam<InType, OutType>> {
 public:
  /// @brief Generates a readable name for the performance test case.
  static std::string CustomPerfTestName(const ::testing::TestParamInfo<PerfTestParam<InType, OutType>> &info) {
    return ppc::performance::GetStringParamName(
               std::get<static_cast<std::size_t>(GTestParamIndex::kTestParams)>(info.param)) +
           "_" + std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(info.param);
  }

 protected:
  virtual bool CheckTestOutputData(OutType &output_data) = 0;
  /// @brief Supplies input data for performance testing.
  virtual InType GetTestInputData() = 0;

  virtual void SetPerfAttributes(ppc::performance::PerfAttr &perf_attrs) {
    if (task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kMPI ||
        task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kALL) {
      const double t0 = GetTimeMPI();
      perf_attrs.current_timer = [t0] { return GetTimeMPI() - t0; };
    } else if (task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kOMP) {
      const double t0 = omp_get_wtime();
      perf_attrs.current_timer = [t0] { return omp_get_wtime() - t0; };
    } else if (task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kSEQ ||
               task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kSTL ||
               task_->GetDynamicTypeOfTask() == ppc::task::TypeOfTask::kTBB) {
      const auto t0 = std::chrono::high_resolution_clock::now();
      perf_attrs.current_timer = [&] {
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
        return static_cast<double>(ns) * 1e-9;
      };
    } else {
      throw std::runtime_error("The task type is not supported for performance testing.");
    }
  }

  void ExecuteTest(const PerfTestParam<InType, OutType> &perf_test_param) {
    auto task_getter = std::get<static_cast<std::size_t>(GTestParamIndex::kTaskGetter)>(perf_test_param);
    auto test_name = std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(perf_test_param);
    auto mode = std::get<static_cast<std::size_t>(GTestParamIndex::kTestParams)>(perf_test_param);

    ASSERT_FALSE(test_name.find("unknown") != std::string::npos);
    if (test_name.find("disabled") != std::string::npos) {
      GTEST_SKIP();
    }

    const auto test_env_scope = ppc::util::test::MakePerTestEnvForCurrentGTest(test_name);

    task_ = task_getter(GetTestInputData());
    ppc::performance::Perf perf(task_);
    ppc::performance::PerfAttr perf_attr;
    SetPerfAttributes(perf_attr);

    if (mode == ppc::performance::PerfResults::TypeOfRunning::kPipeline) {
      perf.PipelineRun(perf_attr);
    } else if (mode == ppc::performance::PerfResults::TypeOfRunning::kTaskRun) {
      perf.TaskRun(perf_attr);
    } else {
      std::stringstream err_msg;
      err_msg << '\n' << "The type of performance check for the task was not selected.\n";
      throw std::runtime_error(err_msg.str().c_str());
    }

    if (GetMPIRank() == 0) {
      perf.PrintPerfStatistic(test_name);
    }

    OutType output_data = task_->GetOutput();
    ASSERT_TRUE(CheckTestOutputData(output_data));
  }

 private:
  ppc::task::TaskPtr<InType, OutType> task_;
};

template <typename TaskType, typename InputType>
auto MakePerfTaskTuples(const std::string &settings_path) {
  const auto name = std::string(GetNamespace<TaskType>()) + "_" +
                    ppc::task::GetStringTaskType(TaskType::GetStaticTypeOfTask(), settings_path);

  return std::make_tuple(std::make_tuple(ppc::task::TaskGetter<TaskType, InputType>, name,
                                         ppc::performance::PerfResults::TypeOfRunning::kPipeline),
                         std::make_tuple(ppc::task::TaskGetter<TaskType, InputType>, name,
                                         ppc::performance::PerfResults::TypeOfRunning::kTaskRun));
}

template <typename Tuple, std::size_t... I>
auto TupleToGTestValuesImpl(const Tuple &tup, std::index_sequence<I...> /*unused*/) {
  return ::testing::Values(std::get<I>(tup)...);
}

template <typename Tuple>
auto TupleToGTestValues(Tuple &&tup) {
  constexpr size_t kSize = std::tuple_size_v<std::decay_t<Tuple>>;
  return TupleToGTestValuesImpl(std::forward<Tuple>(tup), std::make_index_sequence<kSize>{});
}

template <typename InputType, typename... TaskTypes>
auto MakeAllPerfTasks(const std::string &settings_path) {
  return std::tuple_cat(MakePerfTaskTuples<TaskTypes, InputType>(settings_path)...);
}

}  // namespace ppc::util
