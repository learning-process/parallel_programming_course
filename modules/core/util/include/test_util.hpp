#pragma once

#include "core/perf/include/perf.hpp"
#include <gtest/gtest.h>
#include <mpi.h>

namespace ppc::util {

template <typename InType, typename OutType>
using TestParam = std::tuple<ppc::core::PerfResults::TypeOfRunning,
                             std::function<ppc::core::TaskPtr<InType, OutType>(InType)>,
                             std::string>;

template <typename InType, typename OutType>
class BaseRunPerfTests : public ::testing::TestWithParam<TestParam<InType, OutType>> {
 protected:
  virtual void SetPerfAttributes(ppc::core::PerfAttr& perf_attrs) = 0;
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  void ExecuteTest(ppc::core::PerfResults::TypeOfRunning mode,
                   std::function<ppc::core::TaskPtr<InType, OutType>(InType)> task_getter,
                   std::string test_name) {
    task = task_getter(GetTestInputData());
    ppc::core::Perf perf(task);
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
    OutType output_data = task->GetOutput();
    ASSERT_TRUE(CheckTestOutputData(output_data));
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task;
};

#define ADD_MODES(TaskType, InputTypeParam) \
    std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline, \
                    ppc::core::task_getter<TaskType, InputTypeParam>, \
                    ppc::util::GetNamespace<TaskType>()), \
    std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun, \
                    ppc::core::task_getter<TaskType, InputTypeParam>, \
                    ppc::util::GetNamespace<TaskType>())


}  // namespace ppc::util
