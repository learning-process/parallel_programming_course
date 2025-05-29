#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <memory>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/util/include/test_util.hpp"
#include "core/util/include/util.hpp"
#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

using InType = std::vector<int>;
using OutType = std::vector<int>;

class ExampleRunPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
  static constexpr int kCount = 400;
  InType input_data_;

  void SetUp() override {
    input_data_.assign(kCount * kCount, 0);
    for (int i = 0; i < kCount; ++i) {
      input_data_[(i * kCount) + i] = 1;
    }
  }

  void SetPerfAttributes(ppc::core::PerfAttr& perf_attrs) final {
    const auto t0 = std::chrono::high_resolution_clock::now();
    perf_attrs.current_timer = [&] {
      auto now = std::chrono::high_resolution_clock::now();
      auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
      return static_cast<double>(ns) * 1e-9;
    };
  }

  bool CheckTestOutputData(OutType& output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

TEST_P(ExampleRunPerfTest, RunPerfModes) { ExecuteTest(GetParam()); }

INSTANTIATE_TEST_SUITE_P_NOLINT(
    RunModeTests, ExampleRunPerfTest,
    ::testing::Values(ADD_PERF_MODES(nesterov_a_test_task_all::NesterovATestTaskALL, InType),
                      ADD_PERF_MODES(nesterov_a_test_task_mpi::NesterovATestTaskMPI, InType),
                      ADD_PERF_MODES(nesterov_a_test_task_omp::NesterovATestTaskOMP, InType),
                      ADD_PERF_MODES(nesterov_a_test_task_seq::NesterovATestTaskSEQ, InType),
                      ADD_PERF_MODES(nesterov_a_test_task_stl::NesterovATestTaskSTL, InType),
                      ADD_PERF_MODES(nesterov_a_test_task_tbb::NesterovATestTaskTBB, InType)),
    ExampleRunPerfTest::CustomPerfTestName);
