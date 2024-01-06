// Copyright 2023 Nesterov Alexander
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
  auto testTask = std::make_shared<ppc::test::TestTask<uint32_t>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  ASSERT_LE(perfResults->time_sec, 10.0);
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
  auto testTask = std::make_shared<ppc::test::TestTask<float>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  ASSERT_LE(perfResults->time_sec, 10.0);
  EXPECT_EQ(out[0], in.size());
}

TEST(perf_tests, check_perf_pipeline_uint8_t) {
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
  auto testTask = std::make_shared<ppc::test::TestTask<uint8_t>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.pipeline_run(perfAttr, perfResults);

  ASSERT_LE(perfResults->time_sec, 10.0);
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
  auto testTask = std::make_shared<ppc::test::TestTask<uint32_t>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.task_run(perfAttr, perfResults);

  ASSERT_LE(perfResults->time_sec, 10.0);
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
  auto testTask = std::make_shared<ppc::test::TestTask<float>>(taskData);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  ppc::core::Perf perfAnalyzer(testTask);
  perfAnalyzer.task_run(perfAttr, perfResults);

  ASSERT_LE(perfResults->time_sec, 10.0);
  EXPECT_EQ(out[0], in.size());
}
