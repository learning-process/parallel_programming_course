#include <gtest/gtest.h>

#include "example_threads/all/include/ops_all.hpp"
#include "example_threads/common/include/common.hpp"
#include "example_threads/omp/include/ops_omp.hpp"
#include "example_threads/seq/include/ops_seq.hpp"
#include "example_threads/stl/include/ops_stl.hpp"
#include "example_threads/tbb/include/ops_tbb.hpp"
#include "util/include/perf_test_util.hpp"

namespace nesterov_a_test_task_threads {

class ExampleRunPerfTestThreads : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 200;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestThreads, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, NesterovATestTaskALL, NesterovATestTaskOMP, NesterovATestTaskSEQ,
                                NesterovATestTaskSTL, NesterovATestTaskTBB>(PPC_SETTINGS_example_threads);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestThreads::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestThreads, kGtestValues, kPerfTestName);

}  // namespace nesterov_a_test_task_threads
