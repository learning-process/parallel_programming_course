#include <fmt/core.h>
#include <gtest/gtest.h>
#include <omp.h>

#include <cstdio>
#include <memory>
#include <string>
#include <utility>

#include "core/util/include/util.hpp"
#include "mpi.h"
#include "oneapi/tbb/global_control.h"

class UnreadMessagesDetector : public ::testing::EmptyTestEventListener {
 public:
  UnreadMessagesDetector() = default;

  void OnTestEnd(const ::testing::TestInfo& /*test_info*/) override {
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);

    int flag = -1;
    MPI_Status status;

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

    if (flag != 0) {
      fmt::println(
          stderr,
          "[  PROCESS {}  ] [  FAILED  ] {}.{}: MPI message queue has an unread message from process {} with tag {}",
          rank, "test_suite_name", "test_name", status.MPI_SOURCE, status.MPI_TAG);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

 private:
};

class WorkerTestFailurePrinter : public ::testing::EmptyTestEventListener {
 public:
  explicit WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener> base) : base_(std::move(base)) {}

  void OnTestEnd(const ::testing::TestInfo& test_info) override {
    if (test_info.result()->Passed()) {
      return;
    }
    PrintProcessRank();
    base_->OnTestEnd(test_info);
  }

  void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override {
    if (test_part_result.passed() || test_part_result.skipped()) {
      return;
    }
    PrintProcessRank();
    base_->OnTestPartResult(test_part_result);
  }

 private:
  static void PrintProcessRank() {
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    fmt::print(" [  PROCESS {}  ] ", rank);
  }

  std::shared_ptr<::testing::TestEventListener> base_;
};

int main(int argc, char** argv) {
  if (ppc::util::IsUnderMpirun()) {
    MPI_Init(&argc, &argv);

    // Limit the number of threads in TBB
    tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

    ::testing::InitGoogleTest(&argc, argv);

    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0 && (argc < 2 || argv[1] != std::string("--print-workers"))) {
      auto* listener = listeners.Release(listeners.default_result_printer());
      listeners.Append(new WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener)));
    }
    listeners.Append(new UnreadMessagesDetector());
    auto status = RUN_ALL_TESTS();

    MPI_Finalize();
    return status;
  }

  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
