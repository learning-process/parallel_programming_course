#include <gtest/gtest.h>

#include <benchmark/benchmark.h>
#include <benchmark/reporter.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "oneapi/tbb/global_control.h"
#include "runners/include/runners.hpp"
#include "util/include/util.hpp"

namespace {

class NullBenchmarkReporter final : public benchmark::BenchmarkReporter {
 public:
  bool ReportContext(const Context & /*context*/) override {
    return true;
  }

  void ReportRuns(const std::vector<Run> & /*report*/) override {}
};

int RunAllTests() {
  const int status = RUN_ALL_TESTS();
  if (ppc::util::DestructorFailureFlag::Get()) {
    throw std::runtime_error(
        std::format("[  ERROR  ] Destructor failed with code {}", ppc::util::DestructorFailureFlag::Get()));
  }
  return status;
}

void SyncGTestSeed() {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int seed = ::testing::GTEST_FLAG(random_seed);
  if (rank == 0 && seed == 0) {
    try {
      seed = static_cast<int>((std::random_device{}() % 99999U) + 1U);
    } catch (...) {
      seed = 0;
    }
    if (seed == 0) {
      const auto now = static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
      seed = static_cast<int>((now % 99999ULL) + 1ULL);
    }
  }
  MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
  ::testing::GTEST_FLAG(random_seed) = seed;
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

std::vector<std::string> MakeBenchmarkArgs(const char *program_name, int rank) {
  std::vector<std::string> args{program_name != nullptr ? program_name : "ppc_perf_tests"};
  args.emplace_back("--benchmark_format=console");
  args.emplace_back("--benchmark_time_unit=s");

  const auto benchmark_filter = env::get<std::string>("PPC_BENCHMARK_FILTER");
  if (benchmark_filter.has_value()) {
    args.emplace_back(std::string("--benchmark_filter=") + benchmark_filter.value());
  }

  if (rank == 0) {
    const auto benchmark_out = env::get<std::string>("PPC_BENCHMARK_OUT");
    if (benchmark_out.has_value()) {
      const std::filesystem::path out_path(benchmark_out.value());
      if (out_path.has_parent_path()) {
        std::filesystem::create_directories(out_path.parent_path());
      }
      args.emplace_back(std::string("--benchmark_out=") + benchmark_out.value());
      args.emplace_back("--benchmark_out_format=json");
    }
  }

  return args;
}

void InitializeBenchmark(int argc, char **argv, int rank) {
  static std::vector<std::string> benchmark_args;
  static std::vector<char *> benchmark_argv;

  benchmark_args = MakeBenchmarkArgs((argc > 0) ? argv[0] : nullptr, rank);
  benchmark_argv.clear();
  benchmark_argv.reserve(benchmark_args.size());
  for (auto &arg : benchmark_args) {
    benchmark_argv.push_back(arg.data());
  }
  int benchmark_argc = static_cast<int>(benchmark_argv.size());
  benchmark::Initialize(&benchmark_argc, benchmark_argv.data());
}

int RunRegisteredBenchmarks(int rank) {
  ppc::util::PerformanceFailureFlag::Unset();
  if (rank == 0) {
    benchmark::RunSpecifiedBenchmarks();
  } else {
    NullBenchmarkReporter reporter;
    std::ofstream null_stream;
#ifdef _WIN32
    null_stream.open("NUL");
#else
    null_stream.open("/dev/null");
#endif
    if (null_stream.is_open()) {
      reporter.SetOutputStream(&null_stream);
      reporter.SetErrorStream(&null_stream);
    }
    benchmark::RunSpecifiedBenchmarks(&reporter, nullptr);
  }
  const int status = ppc::util::PerformanceFailureFlag::Get() ? EXIT_FAILURE : EXIT_SUCCESS;
  benchmark::Shutdown();
  benchmark::ClearRegisteredBenchmarks();
  return status;
}

int RunAllTestsSafely() {
  try {
    return RunAllTests();
  } catch (const std::exception &e) {
    std::cerr << std::format("[  ERROR  ] Exception after performance tests: {}", e.what()) << '\n';
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[  ERROR  ] Unknown exception after performance tests" << '\n';
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    return EXIT_FAILURE;
  }
}

int SynchronizeStatus(int local_status, std::string_view stage) {
  const int local_failed = (local_status == EXIT_SUCCESS) ? 0 : 1;
  int any_failed = local_failed;
  const int reduce_res = MPI_Allreduce(&local_failed, &any_failed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  if (reduce_res != MPI_SUCCESS) {
    std::cerr << "[  ERROR  ] MPI_Allreduce failed while synchronizing " << stage << " status with code " << reduce_res
              << '\n';
    MPI_Abort(MPI_COMM_WORLD, reduce_res);
    return EXIT_FAILURE;
  }
  return (any_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int RunPerformanceMain(int argc, char **argv) {
  const int init_res = MPI_Init(&argc, &argv);
  if (init_res != MPI_SUCCESS) {
    std::cerr << "[  ERROR  ] MPI_Init failed with code " << init_res << '\n';
    MPI_Abort(MPI_COMM_WORLD, init_res);
    return init_res;
  }

  tbb::global_control control(tbb::global_control::max_allowed_parallelism, ppc::util::GetNumThreads());

  ::testing::InitGoogleTest(&argc, argv);

  SyncGTestSeed();
  SyncGTestFilter();

  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
  const bool print_workers = HasFlag(argc, argv, "--print-workers");
  if (rank != 0 && !print_workers) {
    auto *listener = listeners.Release(listeners.default_result_printer());
    listeners.Append(
        new ppc::runners::WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener)));
  }
  listeners.Append(new ppc::runners::UnreadMessagesDetector());

  int status = SynchronizeStatus(RunAllTestsSafely(), "GTest");
  if (status == EXIT_SUCCESS) {
    InitializeBenchmark(argc, argv, rank);
    status = SynchronizeStatus(RunRegisteredBenchmarks(rank), "Google Benchmark");
  }

  const int finalize_res = MPI_Finalize();
  if (finalize_res != MPI_SUCCESS) {
    std::cerr << "[  ERROR  ] MPI_Finalize failed with code " << finalize_res << '\n';
    MPI_Abort(MPI_COMM_WORLD, finalize_res);
    return finalize_res;
  }
  return status;
}

}  // namespace

int main(int argc, char **argv) {
  try {
    return RunPerformanceMain(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "[  ERROR  ] Unhandled exception in performance tests: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "[  ERROR  ] Unknown unhandled exception in performance tests" << '\n';
  }
  return EXIT_FAILURE;
}
