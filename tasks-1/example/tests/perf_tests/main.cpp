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

using OutputType = std::vector<int>;

using TestParam = std::tuple<ppc::core::PerfResults::TypeOfRunning,
                             std::function<ppc::core::TaskPtr(OutputType)>,
                             std::function<OutputType(ppc::core::TaskPtr)>>;

class ExampleRunPerfTest : public ::testing::TestWithParam<TestParam> {
 protected:
  static constexpr int kCount = 400;
  OutputType input_data;

  void SetUp() override {
    input_data.assign(kCount * kCount, 0);
    for (int i = 0; i < kCount; ++i) {
      input_data[(i * kCount) + i] = 1;
    }
  }

  void ExecuteTest(ppc::core::PerfResults::TypeOfRunning mode,
                   std::function<ppc::core::TaskPtr(OutputType)> task_getter,
                   std::function<OutputType(ppc::core::TaskPtr)> data_getter) {
    auto task = task_getter(input_data);
    ppc::core::Perf perf(task);

    ppc::core::PerfAttr perf_attr;
    const auto t0 = std::chrono::high_resolution_clock::now();
    perf_attr.current_timer = [&] {
      auto now = std::chrono::high_resolution_clock::now();
      auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
      return static_cast<double>(ns) * 1e-9;
    };

    if (mode == ppc::core::PerfResults::TypeOfRunning::kPipeline) {
      perf.PipelineRun(perf_attr);
    } else {
      perf.TaskRun(perf_attr);
    }

    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      perf.PrintPerfStatistic();
    }
    ASSERT_EQ(input_data, data_getter(task));
  }
};

TEST_P(ExampleRunPerfTest, RunModes) {
  ExecuteTest(std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P_NOLINT(
    RunModeTests,
    ExampleRunPerfTest,
    ::testing::Values(
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_all::TestTaskALL, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_all::TestTaskALL, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_all::TestTaskALL, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_all::TestTaskALL, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_mpi::TestTaskMPI, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_mpi::TestTaskMPI, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_mpi::TestTaskMPI, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_mpi::TestTaskMPI, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_omp::TestTaskOpenMP, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_omp::TestTaskOpenMP, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_omp::TestTaskOpenMP, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_omp::TestTaskOpenMP, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_seq::TestTaskSequential, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_seq::TestTaskSequential, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_seq::TestTaskSequential, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_seq::TestTaskSequential, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_stl::TestTaskSTL, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_stl::TestTaskSTL, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_stl::TestTaskSTL, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_stl::TestTaskSTL, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                        ppc::core::task_getter<nesterov_a_test_task_tbb::TestTaskTBB, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_tbb::TestTaskTBB, OutputType>),
        std::make_tuple(ppc::core::PerfResults::TypeOfRunning::kTaskRun,
                        ppc::core::task_getter<nesterov_a_test_task_tbb::TestTaskTBB, OutputType>,
                        ppc::core::data_getter<nesterov_a_test_task_tbb::TestTaskTBB, OutputType>)
        )
);
