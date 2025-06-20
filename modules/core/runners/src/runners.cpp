#include "core/runners/include/runners.hpp"

#include <gtest/gtest.h>
#include <mpi.h>
#include <omp.h>

#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <string>

#include "core/util/include/util.hpp"
#include "oneapi/tbb/global_control.h"

namespace ppc::core {

void UnreadMessagesDetector::OnTestEnd(const ::testing::TestInfo& /*test_info*/) {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier(MPI_COMM_WORLD);

  int flag = -1;
  MPI_Status status;

  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

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

void WorkerTestFailurePrinter::OnTestEnd(const ::testing::TestInfo& test_info) {
  if (test_info.result()->Passed()) {
    return;
  }
  PrintProcessRank();
  base_->OnTestEnd(test_info);
}

void WorkerTestFailurePrinter::OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
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

int Init(int argc, char** argv) {
  const int init_res = MPI_Init(&argc, &argv);
  if (init_res != MPI_SUCCESS) {
    std::cerr << std::format("[  ERROR  ] MPI_Init failed with code {}", init_res) << '\n';
    MPI_Abort(MPI_COMM_WORLD, init_res);
    return init_res;
  }

  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

  ::testing::InitGoogleTest(&argc, argv);

  auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank != 0 && (argc < 2 || argv[1] != std::string("--print-workers"))) {
    auto* listener = listeners.Release(listeners.default_result_printer());
    listeners.Append(new ppc::core::WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener)));
  }
  listeners.Append(new ppc::core::UnreadMessagesDetector());
  auto status = RUN_ALL_TESTS();

  const int finalize_res = MPI_Finalize();
  if (finalize_res != MPI_SUCCESS) {
    std::cerr << std::format("[  ERROR  ] MPI_Finalize failed with code {}", finalize_res) << '\n';
    MPI_Abort(MPI_COMM_WORLD, finalize_res);
    return finalize_res;
  }
  return status;
}

}  // namespace ppc::core
