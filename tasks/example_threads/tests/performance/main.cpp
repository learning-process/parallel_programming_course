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
#include "example_threads/all/include/ops_all.hpp"
#include "example_threads/omp/include/ops_omp.hpp"
#include "example_threads/seq/include/ops_seq.hpp"
#include "example_threads/stl/include/ops_stl.hpp"
#include "example_threads/tbb/include/ops_tbb.hpp"

namespace nesterov_a_test_task_threads {

class ExampleRunPerfTestThreads : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 111;
  InType input_data_{};

  void SetUp() override { input_data_ = kCount_; }

  bool CheckTestOutputData(OutType& output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

INIT_PERF_TASK_GENERATOR(InType, PPC_SETTINGS_example_threads)

using PerfTasks = perf_tasks_type_list<NesterovATestTaskALL, NesterovATestTaskOMP, NesterovATestTaskSEQ,
                                       NesterovATestTaskSTL, NesterovATestTaskTBB>;

const auto kAllPerfTasks = std::tuple_cat(MakeTask(PerfTasks{}));

TEST_P(ExampleRunPerfTestThreads, RunPerfModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(RunModeTests, ExampleRunPerfTestThreads, ppc::util::TupleToGTestValues(kAllPerfTasks),
                                ExampleRunPerfTestThreads::CustomPerfTestName);

}  // namespace nesterov_a_test_task_threads
