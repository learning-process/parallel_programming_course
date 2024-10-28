#include <gtest/gtest.h>

#include <vector>

#include "core/perf/func_tests/test_task.hpp"
#include "core/perf/include/perf.hpp"

TEST(perf_tests, check_perf_pipeline) {
  // Create data
  std::vector<uint32_t> in(2000, 1);
  std::vector<uint32_t> out(1, 0);

  // Create TaskData
  auto taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  auto testTask = std::make_shared<ppc::test::perf::TestTask<uint32_t>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  // Get perf statistic
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_LE(perfResults->time_sec, ppc::core::PerfResults::MAX_TIME);
  EXPECT_EQ(out[0], in.size());
}

TEST(perf_tests, check_perf_pipeline_float) {
  // Create data
  std::vector<float> in(2000, 1);
  std::vector<float> out(1, 0);

  // Create TaskData
  auto taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  auto testTask = std::make_shared<ppc::test::perf::TestTask<float>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  // Get perf statistic
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_LE(perfResults->time_sec, ppc::core::PerfResults::MAX_TIME);
  EXPECT_EQ(out[0], in.size());
}

TEST(perf_tests, check_perf_pipeline_uint8_t_slow_test) {
  // Create data
  std::vector<uint8_t> in(128, 1);
  std::vector<uint8_t> out(1, 0);

  // Create TaskData
  auto taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  auto testTask = std::make_shared<ppc::test::perf::FakePerfTask<uint8_t>>(taskData);
  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 1;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  // Get perf statistic
  ASSERT_ANY_THROW(ppc::core::Perf::print_perf_statistic(perfResults));
  ASSERT_GE(perfResults->time_sec, ppc::core::PerfResults::MAX_TIME);
  EXPECT_EQ(out[0], in.size());
}

TEST(perf_tests, check_perf_task) {
  // Create data
  std::vector<uint32_t> in(2000, 1);
  std::vector<uint32_t> out(1, 0);

  // Create TaskData
  auto taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  auto testTask = std::make_shared<ppc::test::perf::TestTask<uint32_t>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.task_run(perfAttr, perfResults);

  // Get perf statistic
  perfResults->type_of_running = ppc::core::PerfResults::NONE;
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_LE(perfResults->time_sec, ppc::core::PerfResults::MAX_TIME);
  EXPECT_EQ(out[0], in.size());
}

TEST(perf_tests, check_perf_task_float) {
  // Create data
  std::vector<float> in(2000, 1);
  std::vector<float> out(1, 0);

  // Create TaskData
  auto taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  auto testTask = std::make_shared<ppc::test::perf::TestTask<float>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.task_run(perfAttr, perfResults);

  // Get perf statistic
  perfResults->type_of_running = ppc::core::PerfResults::PIPELINE;
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_LE(perfResults->time_sec, ppc::core::PerfResults::MAX_TIME);
  EXPECT_EQ(out[0], in.size());
}
