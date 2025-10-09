#include "runners/include/runners.hpp"

#include <gtest/gtest.h>
#include <mpi.h>

#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "oneapi/tbb/global_control.h"
#include "util/include/util.hpp"

namespace ppc::runners {

void UnreadMessagesDetector::OnTestEnd(const ::testing::TestInfo & /*test_info*/) {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier(MPI_COMM_WORLD);

  int flag = -1;
  MPI_Status status;

  const int iprobe_res = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
  if (iprobe_res != MPI_SUCCESS) {
    std::cerr << std::format("[  PROCESS {}  ] [  ERROR  ] MPI_Iprobe failed with code {}", rank, iprobe_res) << '\n';
    MPI_Abort(MPI_COMM_WORLD, iprobe_res);
  }

  if (flag != 0) {
    std::cerr
        << std::format(
               "[  PROCESS {}  ] [  FAILED  ] MPI message queue has an unread message from process {} with tag {}",
               rank, status.MPI_SOURCE, status.MPI_TAG)
        << '\n';
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  MPI_Barrier(MPI_COMM_WORLD);
}

void WorkerTestFailurePrinter::OnTestEnd(const ::testing::TestInfo &test_info) {
  if (test_info.result()->Passed()) {
    return;
  }
  PrintProcessRank();
  base_->OnTestEnd(test_info);
}

void WorkerTestFailurePrinter::OnTestPartResult(const ::testing::TestPartResult &test_part_result) {
  if (test_part_result.passed() || test_part_result.skipped()) {
    return;
  }
  PrintProcessRank();
  base_->OnTestPartResult(test_part_result);
}

void WorkerTestFailurePrinter::PrintProcessRank() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::cerr << std::format(" [  PROCESS {}  ] ", rank);
}

namespace {
int RunAllTests() {
  auto status = RUN_ALL_TESTS();
  if (ppc::util::DestructorFailureFlag::Get()) {
    throw std::runtime_error(
        std::format("[  ERROR  ] Destructor failed with code {}", ppc::util::DestructorFailureFlag::Get()));
  }
  return status;
}
}  // namespace

int Init(int argc, char **argv) {
  const int init_res = MPI_Init(&argc, &argv);
  if (init_res != MPI_SUCCESS) {
    std::cerr << std::format("[  ERROR  ] MPI_Init failed with code {}", init_res) << '\n';
    MPI_Abort(MPI_COMM_WORLD, init_res);
    return init_res;
  }

  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

  ::testing::InitGoogleTest(&argc, argv);

  auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank != 0 && (argc < 2 || argv[1] != std::string("--print-workers"))) {
    auto *listener = listeners.Release(listeners.default_result_printer());
    listeners.Append(new WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener)));
  }
  listeners.Append(new UnreadMessagesDetector());

  auto status = RunAllTests();

  const int finalize_res = MPI_Finalize();
  if (finalize_res != MPI_SUCCESS) {
    std::cerr << std::format("[  ERROR  ] MPI_Finalize failed with code {}", finalize_res) << '\n';
    MPI_Abort(MPI_COMM_WORLD, finalize_res);
    return finalize_res;
  }
  return status;
}

int SimpleInit(int argc, char **argv) {
  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

  testing::InitGoogleTest(&argc, argv);
  return RunAllTests();
}

}  // namespace ppc::runners
