#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/perf/func_tests/test_task.hpp"
#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"

TEST(perf_tests, check_perf_pipeline) {
  // Create data
  std::vector<uint32_t> in(2000, 1);

  // Create Task
  auto test_task = std::make_shared<ppc::test::perf::TestTask<uint32_t>>(in);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perf_analyzer(test_task);
  perf_analyzer.PipelineRun(perf_attr, perf_results);

  // Get perf statistic
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  ASSERT_LE(perf_results->time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->Get(), in.size());
}

TEST(perf_tests, check_perf_pipeline_float) {
  // Create data
  std::vector<float> in(2000, 1);

  // Create Task
  auto test_task = std::make_shared<ppc::test::perf::TestTask<float>>(in);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perf_analyzer(test_task);
  perf_analyzer.PipelineRun(perf_attr, perf_results);

  // Get perf statistic
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  ASSERT_LE(perf_results->time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->Get(), in.size());
}

TEST(perf_tests, check_perf_pipeline_uint8_t_slow_test) {
  // Create data
  std::vector<uint8_t> in(128, 1);

  // Create Task
  auto test_task = std::make_shared<ppc::test::perf::FakePerfTask<uint8_t>>(in);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 1;

  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perf_analyzer(test_task);
  perf_analyzer.PipelineRun(perf_attr, perf_results);

  // Get perf statistic
  ASSERT_ANY_THROW(ppc::core::Perf::PrintPerfStatistic(perf_results));
}

TEST(perf_tests, check_perf_task) {
  // Create data
  std::vector<uint32_t> in(2000, 1);

  // Create Task
  auto test_task = std::make_shared<ppc::test::perf::TestTask<uint32_t>>(in);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perf_analyzer(test_task);
  perf_analyzer.TaskRun(perf_attr, perf_results);

  // Get perf statistic
  perf_results->type_of_running = ppc::core::PerfResults::kNone;
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  ASSERT_LE(perf_results->time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->Get(), in.size());
}

TEST(perf_tests, check_perf_task_float) {
  // Create data
  std::vector<float> in(2000, 1);

  // Create Task
  auto test_task = std::make_shared<ppc::test::perf::TestTask<float>>(in);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perf_analyzer(test_task);
  perf_analyzer.TaskRun(perf_attr, perf_results);

  // Get perf statistic
  perf_results->type_of_running = ppc::core::PerfResults::kPipeline;
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  ASSERT_LE(perf_results->time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->Get(), in.size());
}
