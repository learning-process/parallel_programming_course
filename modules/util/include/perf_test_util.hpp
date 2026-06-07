#pragma once

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <mpi.h>
#include <omp.h>
#include <tbb/tick_count.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "task/include/task.hpp"
#include "util/include/util.hpp"

namespace ppc::util {

inline double DefaultTimer() {
  return -1.0;
}

struct PerfAttr {
  /// @brief Number of times the task is run for performance evaluation.
  uint64_t num_running = 5;
  /// @brief Timer function returning current time in seconds.
  /// @cond
  std::function<double()> current_timer = DefaultTimer;
  /// @endcond
};

namespace detail {

inline bool ContainsFilterToken(std::string_view value, std::string_view filter) {
  if (filter.empty()) {
    return true;
  }
  return value.contains(filter);
}

inline bool MatchesCategoryFilter(std::string_view task_category, std::string_view category_filter) {
  if (category_filter.empty() || task_category.empty()) {
    return true;
  }
  return category_filter.contains(task_category);
}

inline bool ShouldRunBenchmark(std::string_view test_name, std::string_view task_category) {
  const auto impl_filter = env::get<std::string>("PPC_PERF_IMPL_FILTER");
  const auto category_filter = env::get<std::string>("PPC_PERF_CATEGORY_FILTER");
  const auto impl_filter_value = impl_filter.has_value() ? std::string_view(impl_filter.value()) : std::string_view{};
  const auto category_filter_value =
      category_filter.has_value() ? std::string_view(category_filter.value()) : std::string_view{};
  return ContainsFilterToken(test_name, impl_filter_value) &&
         MatchesCategoryFilter(task_category, category_filter_value);
}

inline std::string GetPerfTaskCategory(std::string_view settings_task_path) {
  if (settings_task_path.starts_with("threads")) {
    return "threads";
  }
  if (settings_task_path.starts_with("processes")) {
    return "processes";
  }
  return {};
}

template <typename InType, typename OutType>
void RunTaskForValidation(const ppc::task::TaskPtr<InType, OutType> &task) {
  task->Validation();
  task->PreProcessing();
  task->Run();
  task->PostProcessing();
}

inline std::function<double()> MakeTechnologyTimer(ppc::task::TypeOfTask task_type) {
  if (task_type == ppc::task::TypeOfTask::kMPI || task_type == ppc::task::TypeOfTask::kALL) {
    return [] { return GetTimeMPI(); };
  }
  if (task_type == ppc::task::TypeOfTask::kOMP) {
    return [] { return omp_get_wtime(); };
  }
  if (task_type == ppc::task::TypeOfTask::kTBB) {
    const auto t0 = tbb::tick_count::now();
    return [t0] { return (tbb::tick_count::now() - t0).seconds(); };
  }
  if (task_type == ppc::task::TypeOfTask::kSEQ || task_type == ppc::task::TypeOfTask::kSTL) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    return [t0] {
      const auto now = std::chrono::high_resolution_clock::now();
      const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
      return static_cast<double>(ns) * 1e-9;
    };
  }
  throw std::runtime_error("The task type is not supported for performance testing.");
}

inline double MaxElapsedTimeAcrossMpiRanks(double elapsed, ppc::task::TypeOfTask task_type) {
  if (task_type != ppc::task::TypeOfTask::kMPI && task_type != ppc::task::TypeOfTask::kALL) {
    return elapsed;
  }
  double max_elapsed = elapsed;
  MPI_Allreduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  return max_elapsed;
}

inline void SkipBenchmarkWithError(benchmark::State &state, const char *message) noexcept {
  try {
    state.SkipWithError(message);
  } catch (const std::exception &e) {
    std::cerr << "Failed to report benchmark error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "Failed to report unknown benchmark error" << '\n';
  }
}

inline void CheckPerfTimeLimit(double elapsed) {
  if (elapsed >= GetPerfMaxTime()) {
    throw std::runtime_error("Task execution time exceeded the performance limit.");
  }
}

template <typename InType, typename OutType>
double RunTaskForBenchmark(const ppc::task::TaskPtr<InType, OutType> &task) {
  const auto task_type = task->GetDynamicTypeOfTask();
  const auto timer = MakeTechnologyTimer(task_type);
  task->GetStateOfTesting() = ppc::task::StateOfTesting::kPerf;

  task->Validation();
  task->PreProcessing();
  SynchronizeMpiRanks();
  const double begin = timer();
  task->Run();
  const double elapsed = timer() - begin;
  task->PostProcessing();
  const double max_elapsed = MaxElapsedTimeAcrossMpiRanks(elapsed, task_type);
  CheckPerfTimeLimit(max_elapsed);
  return max_elapsed;
}

template <typename TaskGetter, typename InType>
void RunBenchmarkBody(const TaskGetter &task_getter, const InType &input_data, const std::string &test_env_token,
                      benchmark::State &state) noexcept {
  try {
    const auto benchmark_env_scope = ppc::util::test::ScopedPerTestEnv(test_env_token);
    for (auto _ : state) {
      auto task = task_getter(input_data);
      const double elapsed = RunTaskForBenchmark(task);
      state.SetIterationTime(elapsed);
      benchmark::DoNotOptimize(task->GetOutput());
    }
  } catch (const std::exception &e) {
    PerformanceFailureFlag::Set();
    SkipBenchmarkWithError(state, e.what());
  } catch (...) {
    PerformanceFailureFlag::Set();
    SkipBenchmarkWithError(state, "Unknown exception in performance benchmark");
  }
}

template <typename TaskGetter, typename InType>
class BenchmarkTaskBody final {
 public:
  BenchmarkTaskBody(TaskGetter task_getter, InType input_data, std::string test_env_token)
      : task_getter_(std::move(task_getter)),
        input_data_(std::move(input_data)),
        test_env_token_(std::move(test_env_token)) {}

  void operator()(benchmark::State &state) const noexcept {
    RunBenchmarkBody(task_getter_, input_data_, test_env_token_, state);
  }

 private:
  TaskGetter task_getter_;
  InType input_data_;
  std::string test_env_token_;
};

}  // namespace detail

template <typename InType, typename OutType>
using PerfTestParam = std::tuple<std::function<ppc::task::TaskPtr<InType, OutType>(InType)>, std::string, std::string>;

template <typename InType, typename OutType>
/// @brief Base class for performance testing of parallel tasks.
/// @tparam InType Input data type.
/// @tparam OutType Output data type.
class BaseRunPerfTests : public ::testing::TestWithParam<PerfTestParam<InType, OutType>> {
 public:
  /// @brief Generates a readable name for the performance test case.
  static std::string CustomPerfTestName(const ::testing::TestParamInfo<PerfTestParam<InType, OutType>> &info) {
    return std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(info.param);
  }

 protected:
  virtual bool CheckTestOutputData(OutType &output_data) = 0;
  /// @brief Supplies input data for performance testing.
  virtual InType GetTestInputData() = 0;

  virtual void SetPerfAttributes(PerfAttr &perf_attrs) {
    perf_attrs.current_timer = detail::MakeTechnologyTimer(task_->GetDynamicTypeOfTask());
  }

  void ExecuteTest(const PerfTestParam<InType, OutType> &perf_test_param) {
    auto task_getter = std::get<static_cast<std::size_t>(GTestParamIndex::kTaskGetter)>(perf_test_param);
    auto test_name = std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(perf_test_param);
    auto task_category = std::get<static_cast<std::size_t>(GTestParamIndex::kTestParams)>(perf_test_param);

    ASSERT_FALSE(test_name.find("unknown") != std::string::npos);
    if (test_name.find("disabled") != std::string::npos) {
      return;
    }
    if (!detail::ShouldRunBenchmark(test_name, task_category)) {
      return;
    }

    const auto test_env_token = ppc::util::test::MakeCurrentGTestToken(test_name);
    const auto test_env_scope = ppc::util::test::ScopedPerTestEnv(test_env_token);

    const auto input_data = GetTestInputData();
    task_ = task_getter(input_data);
    task_->GetStateOfTesting() = ppc::task::StateOfTesting::kPerf;
    SynchronizeMpiRanks();
    detail::RunTaskForValidation(task_);

    OutType output_data = task_->GetOutput();
    ASSERT_TRUE(CheckTestOutputData(output_data));

    PerfAttr perf_attr;
    SetPerfAttributes(perf_attr);
    const auto num_iterations = perf_attr.num_running == 0 ? 1 : perf_attr.num_running;

    using BenchmarkInputType = std::decay_t<decltype(input_data)>;
    auto benchmark_body =
        detail::BenchmarkTaskBody<decltype(task_getter), BenchmarkInputType>(task_getter, input_data, test_env_token);

    benchmark::RegisterBenchmark(test_name, std::move(benchmark_body))
        ->UseManualTime()
        ->Unit(benchmark::kSecond)
        ->Iterations(static_cast<std::int64_t>(num_iterations));
  }

 private:
  ppc::task::TaskPtr<InType, OutType> task_{};
};

template <typename TaskType, typename InputType>
auto MakePerfTaskTuples(const std::string &settings_path, std::string_view settings_task_path = {}) {
  const auto name = std::string(GetNamespace<TaskType>()) + "_" +
                    ppc::task::GetStringTaskType(TaskType::GetStaticTypeOfTask(), settings_path, settings_task_path);

  return std::make_tuple(std::make_tuple(ppc::task::TaskGetter<TaskType, InputType>, name,
                                         detail::GetPerfTaskCategory(settings_task_path)));
}

template <typename Tuple, std::size_t... I>
auto TupleToGTestValuesImpl(const Tuple &tup, std::index_sequence<I...> /*unused*/) {
  return ::testing::Values(std::get<I>(tup)...);
}

template <typename Tuple>
auto TupleToGTestValues(Tuple &&tup) {
  constexpr std::size_t kSize{std::tuple_size_v<std::decay_t<Tuple>>};
  return TupleToGTestValuesImpl(std::forward<Tuple>(tup), std::make_index_sequence<kSize>{});
}

template <typename InputType, typename... TaskTypes>
auto MakeAllPerfTasks(const std::string &settings_path, std::string_view settings_task_path = {}) {
  return std::tuple_cat(MakePerfTaskTuples<TaskTypes, InputType>(settings_path, settings_task_path)...);
}

}  // namespace ppc::util
