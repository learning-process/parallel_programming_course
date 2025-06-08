#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/util/include/perf_test_util.hpp"
#include "core/util/include/util.hpp"
#include "example_processes/mpi/include/ops_mpi.hpp"
#include "example_processes/seq/include/ops_seq.hpp"

namespace nesterov_a_test_task_processes {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 111;
  InType input_data_{};

  void SetUp() override { input_data_ = kCount_; }

  bool CheckTestOutputData(OutType& output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

INIT_PERF_TASK_GENERATOR(InType, PPC_SETTINGS_example_processes)

using PerfTasks = perf_tasks_type_list<NesterovATestTaskMPI, NesterovATestTaskSEQ>;

const auto kAllPerfTasks = std::tuple_cat(MakeTask(PerfTasks{}));

TEST_P(ExampleRunPerfTestProcesses, RunPerfModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(RunModeTests, ExampleRunPerfTestProcesses, ppc::util::TupleToGTestValues(kAllPerfTasks),
                                ExampleRunPerfTestProcesses::CustomPerfTestName);

}  // namespace nesterov_a_test_task_processes
