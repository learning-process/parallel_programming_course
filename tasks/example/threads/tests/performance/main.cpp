#include <gtest/gtest.h>

#include <tuple>

#include "example/common/include/common.hpp"
#include "example/threads/all/include/ops_all.hpp"
#include "example/threads/omp/include/ops_omp.hpp"
#include "example/threads/seq/include/ops_seq.hpp"
#include "example/threads/stl/include/ops_stl.hpp"
#include "example/threads/tbb/include/ops_tbb.hpp"
#include "util/include/perf_test_util.hpp"

namespace example_threads {

class ExampleRunPerfTestThreads : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  const int kCount_ = 200;
  InType input_data_{};
};

namespace {

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, NesterovATestTaskALL, NesterovATestTaskOMP, NesterovATestTaskSEQ,
                                NesterovATestTaskSTL, NesterovATestTaskTBB>(PPC_SETTINGS_example, "threads");

}  // namespace

TEST_F(ExampleRunPerfTestThreads, RunPerf) {
  std::apply([this](const auto &...test_params) { (ExecuteTest(test_params), ...); }, kAllPerfTasks);
}

}  // namespace example_threads
