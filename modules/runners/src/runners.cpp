#include "runners/include/runners.hpp"

#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>

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
  // Abort the whole MPI job on any test failure to avoid other ranks hanging on barriers.
  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
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

void SyncGTestSeed() {
  unsigned int seed = 0;
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    try {
      seed = std::random_device{}();
    } catch (...) {
      seed = 0;
    }
    if (seed == 0) {
      const auto now = static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
      seed = static_cast<unsigned int>(((now & 0x7fffffffULL) | 1ULL));
    }
  }
  MPI_Bcast(&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  ::testing::GTEST_FLAG(random_seed) = static_cast<int>(seed);
}

void SyncGTestFilter() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string filter = (rank == 0) ? ::testing::GTEST_FLAG(filter) : std::string{};
  int len = static_cast<int>(filter.size());
  MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank != 0) {
    filter.resize(static_cast<std::size_t>(len));
  }
  if (len > 0) {
    MPI_Bcast(filter.data(), len, MPI_CHAR, 0, MPI_COMM_WORLD);
  }
  ::testing::GTEST_FLAG(filter) = filter;
}

bool HasFlag(int argc, char **argv, std::string_view flag) {
  for (int i = 1; i < argc; ++i) {
    if (argv[i] != nullptr && std::string_view(argv[i]) == flag) {
      return true;
    }
  }
  return false;
}

int RunAllTestsSafely() {
  try {
    return RunAllTests();
  } catch (const std::exception &e) {
    std::cerr << std::format("[  ERROR  ] Exception after tests: {}", e.what()) << '\n';
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[  ERROR  ] Unknown exception after tests" << '\n';
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    return EXIT_FAILURE;
  }
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

  // Synchronize GoogleTest internals across ranks to avoid divergence
  SyncGTestSeed();
  SyncGTestFilter();

  auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  const bool print_workers = HasFlag(argc, argv, "--print-workers");
  if (rank != 0 && !print_workers) {
    auto *listener = listeners.Release(listeners.default_result_printer());
    listeners.Append(new WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener)));
  }
  listeners.Append(new UnreadMessagesDetector());

  const int status = RunAllTestsSafely();

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
