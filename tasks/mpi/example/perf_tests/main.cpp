#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"
#include "mpi/example/include/ops_mpi.hpp"

class NesterovATaskMPITest : public ::testing::TestWithParam<ppc::core::PerfResults::TypeOfRunning> {
 protected:
  static void RunTest(ppc::core::PerfResults::TypeOfRunning mode) {
    constexpr int kCount = 500;

    // Create data
    std::vector<int> in(kCount * kCount, 0);
    for (size_t i = 0; i < kCount; i++) {
      in[(i * kCount) + i] = 1;
    }

    // Create Task
    auto test_task_mpi = std::make_shared<nesterov_a_test_task_mpi::TestTaskMPI>(in);

    // Create Perf attributes
    ppc::core::PerfAttr perf_attr;
    const auto t0 = std::chrono::high_resolution_clock::now();
    perf_attr.current_timer = [&] {
      auto current_time_point = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
      return static_cast<double>(duration) * 1e-9;
    };

    // Create and init perf results
    ppc::core::PerfResults perf_results;

    // Create Perf analyzer
    ppc::core::Perf perf_analyzer(test_task_mpi);

    if (mode == ppc::core::PerfResults::TypeOfRunning::kPipeline) {
      perf_analyzer.PipelineRun(perf_attr, perf_results);
    } else {
      perf_analyzer.TaskRun(perf_attr, perf_results);
    }

    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      ppc::core::Perf::PrintPerfStatistic(perf_results);
    }

    ASSERT_EQ(in, test_task_mpi->Get());
  }
};

TEST_P(NesterovATaskMPITest, RunModes) { RunTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P(NesterovATests, NesterovATaskMPITest,
                         ::testing::Values(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                                           ppc::core::PerfResults::TypeOfRunning::kTaskRun));
