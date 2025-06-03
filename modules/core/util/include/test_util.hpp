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
#include "nlohmann/json.hpp"

namespace ppc::util {

enum GTestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

template <typename InType, typename OutType, typename TestType = void>
using FuncTestParam = std::tuple<std::function<ppc::core::TaskPtr<InType, OutType>(InType)>, std::string, TestType>;

template <typename InType, typename OutType, typename TestType = void>
using GTestFuncParam = ::testing::TestParamInfo<FuncTestParam<InType, OutType, TestType>>;

template <typename InType, typename OutType>
using PerfTestParam = FuncTestParam<InType, OutType, ppc::core::PerfResults::TypeOfRunning>;

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

    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      perf.PrintPerfStatistic(test_name);
    }
    OutType output_data = task_->GetOutput();
    ASSERT_TRUE(CheckTestOutputData(output_data));
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task_;
};

#define ADD_PERF_TASK(TaskType, InputTypeParam)                                                               \
  std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                             \
                  std::string(ppc::util::GetNamespace<TaskType>()) + std::string("_") +                        \
                      ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), PPC_STUDENT_SETTINGS),     \
                  ppc::core::PerfResults::TypeOfRunning::kPipeline),                                           \
      std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>,                                         \
                      std::string(ppc::util::GetNamespace<TaskType>()) + std::string("_") +                    \
                          ppc::core::GetStringTaskType(TaskType::GetStaticTypeOfTask(), PPC_STUDENT_SETTINGS), \
                      ppc::core::PerfResults::TypeOfRunning::kTaskRun)

template <typename T, typename TestType>
concept HasPrintTestParam = requires(TestType value) {
  { T::PrintTestParam(value) } -> std::same_as<std::string>;
};

template <typename InType, typename OutType, typename TestType = void>
class BaseRunFuncTests : public ::testing::TestWithParam<FuncTestParam<InType, OutType, TestType>> {
 public:
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  template <typename Derived>
  static void RequireStaticInterface() {
    static_assert(HasPrintTestParam<Derived, TestType>,
                  "Derived class must implement: static std::string PrintTestParam(TestType)");
  }

  template <typename Derived>
  static std::string PrintFuncTestName(const GTestFuncParam<InType, OutType, TestType>& info) {
    RequireStaticInterface<Derived>();
    TestType test_param = std::get<ppc::util::GTestParamIndex::kTestParams>(info.param);
    return std::get<GTestParamIndex::kNameTest>(info.param) + "_" + Derived::PrintTestParam(test_param);
  }

 protected:
  void ExecuteTest(FuncTestParam<InType, OutType, TestType> test_param) {
    ASSERT_FALSE(std::get<GTestParamIndex::kNameTest>(test_param).find("unknown") != std::string::npos);
    if (std::get<GTestParamIndex::kNameTest>(test_param).find("disabled") != std::string::npos) {
      GTEST_SKIP();
    }

    task_ = std::get<GTestParamIndex::kTaskGetter>(test_param)(GetTestInputData());
    ASSERT_TRUE(task_->Validation());
    ASSERT_TRUE(task_->PreProcessing());
    ASSERT_TRUE(task_->Run());
    ASSERT_TRUE(task_->PostProcessing());
    ASSERT_TRUE(CheckTestOutputData(task_->GetOutput()));
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task_;
};

template <typename Tuple, std::size_t... Is>
auto ExpandToValuesImpl(const Tuple& t, std::index_sequence<Is...> /*unused*/) {
  return ::testing::Values(std::get<Is>(t)...);
}

template <typename Tuple>
auto ExpandToValues(const Tuple& t) {
  constexpr std::size_t kN = std::tuple_size_v<Tuple>;
  return ExpandToValuesImpl(t, std::make_index_sequence<kN>{});
}

#define INIT_TASK_GENERATOR(InTypeParam, SizesParam)                                                         \
  template <typename Task, std::size_t... Is>                                                                \
  auto GenTaskTuplesImpl(std::index_sequence<Is...>) {                                                       \
    return std::make_tuple(                                                                                  \
        std::make_tuple(ppc::core::TaskGetter<Task, InTypeParam>,                                            \
                        std::string(ppc::util::GetNamespace<Task>()) + std::string("_") +                    \
                            ppc::core::GetStringTaskType(Task::GetStaticTypeOfTask(), PPC_STUDENT_SETTINGS), \
                        SizesParam[Is])...);                                                                 \
  }                                                                                                          \
                                                                                                             \
  template <typename Task>                                                                                   \
  auto TaskListGenerator() {                                                                                 \
    return GenTaskTuplesImpl<Task>(std::make_index_sequence<SizesParam.size()>{});                           \
  }

}  // namespace ppc::util
