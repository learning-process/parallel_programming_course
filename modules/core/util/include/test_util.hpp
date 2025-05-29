#pragma once

#include <gtest/gtest.h>
#include <mpi.h>

#include "core/perf/include/perf.hpp"

namespace ppc::util {

enum TestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

inline std::string GetStringParamName(ppc::core::PerfResults::TypeOfRunning type_of_running) {
  if (type_of_running == core::PerfResults::kTaskRun) {
    return "task_run";
  }
  if (type_of_running == core::PerfResults::kPipeline) {
    return "pipeline";
  }
  return "none";
}

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
    return ppc::util::GetStringParamName(std::get<TestParamIndex::kTestParams>(info.param)) + "_" +
           std::get<TestParamIndex::kNameTest>(info.param);
  }

 protected:
  virtual void SetPerfAttributes(ppc::core::PerfAttr& perf_attrs) = 0;
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  void ExecuteTest(const PerfTestParam<InType, OutType>& perf_test_param) {
    auto task_getter = std::get<TestParamIndex::kTaskGetter>(perf_test_param);
    auto test_name = std::get<TestParamIndex::kNameTest>(perf_test_param);
    auto mode = std::get<TestParamIndex::kTestParams>(perf_test_param);

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

#define ADD_PERF_MODES(TaskType, InputTypeParam)                                                            \
  std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>, ppc::util::GetNamespace<TaskType>(),     \
                  ppc::core::PerfResults::TypeOfRunning::kPipeline),                                        \
      std::make_tuple(ppc::core::TaskGetter<TaskType, InputTypeParam>, ppc::util::GetNamespace<TaskType>(), \
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
  static void require_static_interface() {
    static_assert(HasPrintTestParam<Derived, TestType>,
                  "Derived class must implement: static std::string PrintTestParam(TestType)");
  }

  template <typename Derived>
  static std::string PrintFuncTestName(const GTestFuncParam<InType, OutType, TestType>& info) {
    require_static_interface<Derived>();
    TestType test_param = std::get<ppc::util::TestParamIndex::kTestParams>(info.param);
    return std::get<TestParamIndex::kNameTest>(info.param) + "_" + Derived::PrintTestParam(test_param);
  }

 protected:
  void ExecuteTest(FuncTestParam<InType, OutType, TestType> test_param) {
    task_ = std::get<TestParamIndex::kTaskGetter>(test_param)(GetTestInputData());
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

#define INIT_TASK_GENERATOR(InTypeParam, SizesParam)                                                                  \
  template <typename Task, std::size_t... Is>                                                                         \
  auto GenTaskTuplesImpl(std::index_sequence<Is...>) {                                                                \
    return std::make_tuple(std::make_tuple(ppc::core::TaskGetter<Task, InTypeParam>, ppc::util::GetNamespace<Task>(), \
                                           SizesParam[Is])...);                                                       \
  }                                                                                                                   \
                                                                                                                      \
  template <typename Task>                                                                                            \
  auto TaskListGenerator() {                                                                                          \
    return GenTaskTuplesImpl<Task>(std::make_index_sequence<SizesParam.size()>{});                                    \
  }

}  // namespace ppc::util
