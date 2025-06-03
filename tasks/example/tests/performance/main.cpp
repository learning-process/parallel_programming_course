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

TEST_P(ExampleRunPerfTest, RunPerfModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(
    RunModeTests, ExampleRunPerfTest,
    ::testing::Values(ADD_PERF_TASK(NesterovATestTaskALL, InType), ADD_PERF_TASK(NesterovATestTaskMPI, InType),
                      ADD_PERF_TASK(NesterovATestTaskOMP, InType), ADD_PERF_TASK(NesterovATestTaskSEQ, InType),
                      ADD_PERF_TASK(NesterovATestTaskSTL, InType), ADD_PERF_TASK(NesterovATestTaskTBB, InType)),
    ExampleRunPerfTest::CustomPerfTestName);

}  // namespace nesterov_a_test_task
