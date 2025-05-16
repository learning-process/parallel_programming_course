#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <memory>
#include <vector>

#include "all/example/include/ops_all.hpp"
#include "core/perf/include/perf.hpp"
#include "core/util/include/util.hpp"

class NesterovAllRunTest : public ::testing::TestWithParam<ppc::core::PerfResults::TypeOfRunning> {
 protected:
  static constexpr int kCount = 400;
  std::vector<int> input_data;

  void SetUp() override {
    input_data.assign(kCount * kCount, 0);
    for (int i = 0; i < kCount; ++i) {
      input_data[(i * kCount) + i] = 1;
    }
  }

  void ExecuteTest(ppc::core::PerfResults::TypeOfRunning mode) {
    auto task = std::make_shared<nesterov_a_test_task_all::TestTaskALL>(input_data);
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

    ASSERT_EQ(input_data, task->Get());
  }
};

TEST_P(NesterovAllRunTest, RunModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(RunModeTests, NesterovAllRunTest,
                                ::testing::Values(ppc::core::PerfResults::TypeOfRunning::kPipeline,
                                                  ppc::core::PerfResults::TypeOfRunning::kTaskRun));
