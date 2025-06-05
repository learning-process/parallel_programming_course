#pragma once

#include <gtest/gtest.h>
#include <mpi.h>
#include <omp.h>
#include <tbb/tick_count.h>

#include <csignal>
#include <filesystem>
#include <fstream>

#include "core/perf/include/perf.hpp"
#include "core/util/include/util.hpp"

namespace ppc::util {

template <typename InType, typename OutType>
using PerfTestParam = std::tuple<std::function<ppc::core::TaskPtr<InType, OutType>(InType)>, std::string,
                                 ppc::core::PerfResults::TypeOfRunning>;

template <typename InType, typename OutType>
class BaseRunPerfTests : public ::testing::TestWithParam<PerfTestParam<InType, OutType>> {
 public:
  static std::string CustomPerfTestName(const ::testing::TestParamInfo<PerfTestParam<InType, OutType>>& info) {
    return ppc::core::GetStringParamName(std::get<GTestParamIndex::kTestParams>(info.param)) + "_" +
           std::get<GTestParamIndex::kNameTest>(info.param);
  }

 protected:
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  virtual void SetPerfAttributes(ppc::core::PerfAttr& perf_attrs) {
    if (task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kTBB) {
      const tbb::tick_count t0 = tbb::tick_count::now();
      perf_attrs.current_timer = [t0] { return (tbb::tick_count::now() - t0).seconds(); };
    } else if (task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kMPI ||
               task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kALL) {
      const double t0 = MPI_Wtime();
      perf_attrs.current_timer = [t0] { return MPI_Wtime() - t0; };
    } else if (task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kOMP) {
      const double t0 = omp_get_wtime();
      perf_attrs.current_timer = [t0] { return omp_get_wtime() - t0; };
    } else if (task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kSEQ ||
               task_->GetDynamicTypeOfTask() == ppc::core::TypeOfTask::kSTL) {
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

  void ExecuteTest(const PerfTestParam<InType, OutType>& perf_test_param) {
    auto task_getter = std::get<GTestParamIndex::kTaskGetter>(perf_test_param);
    auto test_name = std::get<GTestParamIndex::kNameTest>(perf_test_param);
    auto mode = std::get<GTestParamIndex::kTestParams>(perf_test_param);

    ASSERT_FALSE(test_name.find("unknown") != std::string::npos);
    if (test_name.find("disabled") != std::string::npos) {
      GTEST_SKIP();
    }

    task_ = task_getter(GetTestInputData());
    ppc::core::Perf perf(task_);
    ppc::core::PerfAttr perf_attr;
    SetPerfAttributes(perf_attr);

    if (mode == ppc::core::PerfResults::TypeOfRunning::kPipeline) {
      perf.PipelineRun(perf_attr);
    } else if (mode == ppc::core::PerfResults::TypeOfRunning::kTaskRun) {
      perf.TaskRun(perf_attr);
    } else {
      std::stringstream err_msg;
      err_msg << '\n' << "The type of performance check for the task was not selected.\n";
      throw std::runtime_error(err_msg.str().c_str());
    }

#ifndef PPC_ASAN_RUN
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      perf.PrintPerfStatistic(test_name);
    }
#else
    perf.PrintPerfStatistic(test_name);
#endif

    OutType output_data = task_->GetOutput();
    ASSERT_TRUE(CheckTestOutputData(output_data));
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task_;
};

#define ADD_PERF_TASK_THREADS(TaskType, InputTypeParam, SettingsPath)                                         \
  std::tuple(std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                 \
                             std::string(ppc::util::GetNamespace<TaskType>()) + "_" +                         \
                                 ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), SettingsPath), \
                             ppc::core::PerfResults::TypeOfRunning::kPipeline),                               \
             std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                 \
                             std::string(ppc::util::GetNamespace<TaskType>()) + "_" +                         \
                                 ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), SettingsPath), \
                             ppc::core::PerfResults::TypeOfRunning::kTaskRun))

template <typename Tuple, std::size_t... I>
auto TupleToGTestValuesImpl(Tuple&& tup, std::index_sequence<I...>) {
  return ::testing::Values(std::get<I>(std::forward<Tuple>(tup))...);
}

template <typename Tuple>
auto TupleToGTestValues(Tuple&& tup) {
  constexpr size_t size = std::tuple_size<std::decay_t<Tuple>>::value;
  return TupleToGTestValuesImpl(std::forward<Tuple>(tup), std::make_index_sequence<size>{});
}

#ifndef PPC_ASAN_RUN
#define ADD_PERF_TASK_PROCESS(TaskType, InputTypeParam, SettingsPath)                                         \
  std::tuple(std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                 \
                             std::string(ppc::util::GetNamespace<TaskType>()) + "_" +                         \
                                 ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), SettingsPath), \
                             ppc::core::PerfResults::TypeOfRunning::kPipeline),                               \
             std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                 \
                             std::string(ppc::util::GetNamespace<TaskType>()) + "_" +                         \
                                 ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), SettingsPath), \
                             ppc::core::PerfResults::TypeOfRunning::kTaskRun))
#else
#define ADD_PERF_TASK_PROCESS(TaskType, InputTypeParam, SettingsPath) std::make_tuple()
#endif

}  // namespace ppc::util
