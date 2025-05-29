#pragma once

#include <gtest/gtest.h>
#include <mpi.h>

#include "core/perf/include/perf.hpp"

namespace ppc::util {

enum FuncTestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

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

template <typename InType, typename OutType>
using PerfTestParam = FuncTestParam<InType, OutType, ppc::core::PerfResults::TypeOfRunning>;

template <typename InType, typename OutType>
class BaseRunPerfTests : public ::testing::TestWithParam<PerfTestParam<InType, OutType>> {
 public:
  static std::string CustomPerfTestName(const ::testing::TestParamInfo<PerfTestParam<InType, OutType>>& info) {
    return ppc::util::GetStringParamName(std::get<FuncTestParamIndex::kTestParams>(info.param)) + "_" +
           std::get<FuncTestParamIndex::kNameTest>(info.param);
  }

 protected:
  virtual void SetPerfAttributes(ppc::core::PerfAttr& perf_attrs) = 0;
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  void ExecuteTest(const PerfTestParam<InType, OutType>& perfTestParam) {
    auto task_getter = std::get<ppc::util::FuncTestParamIndex::kTaskGetter>(perfTestParam);
    auto test_name = std::get<ppc::util::FuncTestParamIndex::kNameTest>(perfTestParam);
    auto mode = std::get<ppc::util::FuncTestParamIndex::kTestParams>(perfTestParam);

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

template <typename InType, typename OutType, typename TestType = void>
class BaseRunFuncTests : public ::testing::TestWithParam<ppc::util::FuncTestParam<InType, OutType, TestType>> {
 public:
  virtual void CheckTestOutputData() = 0;
  ppc::core::TaskPtr<InType, OutType>& GetTaskPtr() { return task; }
  InType& GetTestInput() { return test_input_; }

 protected:
  void ExecuteTest() {
    ASSERT_TRUE(task->Validation());
    ASSERT_TRUE(task->PreProcessing());
    ASSERT_TRUE(task->Run());
    ASSERT_TRUE(task->PostProcessing());
    CheckTestOutputData();
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task;
  InType test_input_;
};

}  // namespace ppc::util
