#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <memory>
#include <vector>

#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

#include "core/perf/include/perf.hpp"
#include "core/util/include/util.hpp"

template <typename OutputTypeParameter>
using TestParam = std::tuple<ppc::core::PerfResults::TypeOfRunning,
                             std::function<ppc::core::TaskPtr(OutputTypeParameter)>,
                             std::function<OutputTypeParameter(ppc::core::TaskPtr)>,
                             std::string>;

template <typename OutputTypeParameter>
class BaseRunPerfTests : public ::testing::TestWithParam<TestParam<OutputTypeParameter>> {
 protected:
  virtual void setPerfAttributes(ppc::core::PerfAttr& perf_attrs) = 0;
  virtual bool checkData(std::function<OutputTypeParameter(ppc::core::TaskPtr)> data_getter) = 0;
  virtual OutputTypeParameter getInputData() = 0;
  ppc::core::TaskPtr task;

  void ExecuteTest(ppc::core::PerfResults::TypeOfRunning mode,
                   std::function<ppc::core::TaskPtr(OutputTypeParameter)> task_getter,
                   std::function<OutputTypeParameter(ppc::core::TaskPtr)> data_getter,
                   std::string test_name) {
    task = task_getter(getInputData());
    ppc::core::Perf perf(task);
    ppc::core::PerfAttr perf_attr;
    setPerfAttributes(perf_attr);

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

    ASSERT_TRUE(checkData(data_getter));
  }
};

#define ADD_MODES(TaskType, OutputTypeParam) \
    std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline, \
                    ppc::core::task_getter<TaskType, OutputTypeParam>, \
                    ppc::core::data_getter<TaskType, OutputTypeParam>,\
                    ppc::util::get_namespace<TaskType>()), \
    std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun, \
                    ppc::core::task_getter<TaskType, OutputTypeParam>, \
                    ppc::core::data_getter<TaskType, OutputTypeParam>,\
                    ppc::util::get_namespace<TaskType>())


using OutputType = std::vector<int>;

class ExampleRunPerfTest : public BaseRunPerfTests<OutputType> {
  static constexpr int kCount = 400;
  OutputType input_data;

  void SetUp() override {
    input_data.assign(kCount * kCount, 0);
    for (int i = 0; i < kCount; ++i) {
      input_data[(i * kCount) + i] = 1;
    }
  }

  void setPerfAttributes(ppc::core::PerfAttr& perf_attrs) final {
    const auto t0 = std::chrono::high_resolution_clock::now();
    perf_attrs.current_timer = [&] {
      auto now = std::chrono::high_resolution_clock::now();
      auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
      return static_cast<double>(ns) * 1e-9;
    };
  }

  bool checkData(std::function<OutputType(ppc::core::TaskPtr)> data_getter) final {
    return input_data == data_getter(task);
  }

  OutputType getInputData() final {
    return input_data;
  }
};

TEST_P(ExampleRunPerfTest, RunModes) {
  ExecuteTest(std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), std::get<3>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P_NOLINT(
    RunModeTests,
    ExampleRunPerfTest,
    ::testing::Values(
        ADD_MODES(nesterov_a_test_task_all::TestTaskALL,        OutputType),
        ADD_MODES(nesterov_a_test_task_mpi::TestTaskMPI,        OutputType),
        ADD_MODES(nesterov_a_test_task_omp::TestTaskOpenMP,     OutputType),
        ADD_MODES(nesterov_a_test_task_seq::TestTaskSequential, OutputType),
        ADD_MODES(nesterov_a_test_task_stl::TestTaskSTL,        OutputType),
        ADD_MODES(nesterov_a_test_task_tbb::TestTaskTBB,        OutputType))
);
