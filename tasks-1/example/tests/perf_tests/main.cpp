#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <memory>
#include <vector>

#include "example/all/include/ops_all.hpp"
#include "core/perf/include/perf.hpp"
#include "core/util/include/util.hpp"

using TestParam = std::tuple<ppc::core::PerfResults::TypeOfRunning,
                             std::function<std::shared_ptr<ppc::core::Task>(std::vector<int>)>,
                             std::function<std::vector<int>(std::shared_ptr<ppc::core::Task>)>>;

class NesterovAllRunTest : public ::testing::TestWithParam<TestParam> {
 protected:
  static constexpr int kCount = 400;
  std::vector<int> input_data;

  void SetUp() override {
    input_data.assign(kCount * kCount, 0);
    for (int i = 0; i < kCount; ++i) {
      input_data[(i * kCount) + i] = 1;
    }
  }

  void ExecuteTest(ppc::core::PerfResults::TypeOfRunning mode,
                   std::function<std::shared_ptr<ppc::core::Task>(std::vector<int>)> task_getter,
                   std::function<std::vector<int>(std::shared_ptr<ppc::core::Task>)> data_getter) {
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

TEST_P(NesterovAllRunTest, RunModes) {
  ExecuteTest(std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P_NOLINT(
    RunModeTests,
    NesterovAllRunTest,
    ::testing::Values(
        std::make_tuple(
            ppc::core::PerfResults::TypeOfRunning::kPipeline,
            [](std::vector<int> in) -> std::shared_ptr<nesterov_a_test_task_all::TestTaskALL> {
              return std::make_shared<nesterov_a_test_task_all::TestTaskALL>(in);
            },
            [](std::shared_ptr<ppc::core::Task> current_task) -> std::vector<int> {
              auto inheritance_task = std::dynamic_pointer_cast<nesterov_a_test_task_all::TestTaskALL>(current_task);
              return ppc::core::Task::Get(*inheritance_task, &nesterov_a_test_task_all::TestTaskALL::output_);
            }
            ),
        std::make_tuple(
            ppc::core::PerfResults::TypeOfRunning::kTaskRun,
            [](std::vector<int> in) -> std::shared_ptr<nesterov_a_test_task_all::TestTaskALL> {
              return std::make_shared<nesterov_a_test_task_all::TestTaskALL>(in);
            },
            [](std::shared_ptr<ppc::core::Task> current_task) -> std::vector<int> {
              auto inheritance_task = std::dynamic_pointer_cast<nesterov_a_test_task_all::TestTaskALL>(current_task);
              return ppc::core::Task::Get(*inheritance_task, &nesterov_a_test_task_all::TestTaskALL::output_);
            }
            )
            )
);
