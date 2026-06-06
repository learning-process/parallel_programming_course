#include <gtest/gtest.h>

#include <tuple>

#include "example/common/include/common.hpp"
#include "example/processes/t1/mpi/include/ops_mpi.hpp"
#include "example/processes/t1/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace example_processes_t1 {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
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
  const int kCount_ = 100;
  InType input_data_{};
};

namespace {

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, NesterovATestTaskMPI, NesterovATestTaskSEQ>(
    PPC_SETTINGS_example, "processes.t1");

}  // namespace

TEST_F(ExampleRunPerfTestProcesses, RunPerf) {
  std::apply([this](const auto &...test_params) { (ExecuteTest(test_params), ...); }, kAllPerfTasks);
}

}  // namespace example_processes_t1
