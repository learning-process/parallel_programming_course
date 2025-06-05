#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/util/include/perf_test_util.hpp"
#include "core/util/include/util.hpp"
#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

namespace nesterov_a_test_task {

class ExampleRunPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const std::vector<int>::size_type kCount_ = 111;
  InType input_data_{};

  void SetUp() override { input_data_ = kCount_; }

  bool CheckTestOutputData(OutType& output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

const auto all_perf_tasks = std::tuple_cat(ADD_PERF_TASK_PROCESS(NesterovATestTaskALL, InType, PPC_SETTINGS_example),
                                           ADD_PERF_TASK_PROCESS(NesterovATestTaskMPI, InType, PPC_SETTINGS_example),
                                           ADD_PERF_TASK_THREADS(NesterovATestTaskOMP, InType, PPC_SETTINGS_example),
                                           ADD_PERF_TASK_THREADS(NesterovATestTaskSEQ, InType, PPC_SETTINGS_example),
                                           ADD_PERF_TASK_THREADS(NesterovATestTaskSTL, InType, PPC_SETTINGS_example),
                                           ADD_PERF_TASK_THREADS(NesterovATestTaskTBB, InType, PPC_SETTINGS_example));

TEST_P(ExampleRunPerfTest, RunPerfModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(RunModeTests, ExampleRunPerfTest, ppc::util::TupleToGTestValues(all_perf_tasks),
                                ExampleRunPerfTest::CustomPerfTestName);

}  // namespace nesterov_a_test_task
