// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>

#include <vector>

#include "core/task/include/task.hpp"
#include "ref/average_of_vector_elements/include/ref_task.hpp"

TEST(average_of_vector_elements, check_int32_t) {
  // Create data
  std::vector<int32_t> in(1256, 1);
  std::vector<double> out(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<int32_t, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, true);
  testTask.pre_processing();
  testTask.run();
  testTask.post_processing();
  EXPECT_NEAR(out[0], 1.0, 1e-5);
}

TEST(average_of_vector_elements, check_validate_func) {
  // Create data
  std::vector<int32_t> in(125, 1);
  std::vector<double> out(2, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<int32_t, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, false);
}

TEST(average_of_vector_elements, check_double) {
  // Create data
  std::vector<double> in(25680, 1.123);
  std::vector<double> out(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<double, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, true);
  testTask.pre_processing();
  testTask.run();
  testTask.post_processing();
  EXPECT_NEAR(out[0], 1.123, 1e-5);
}

TEST(average_of_vector_elements, check_uint8_t) {
  // Create data
  std::vector<uint8_t> in(255, 2);
  std::vector<double> out(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<uint8_t, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, true);
  testTask.pre_processing();
  testTask.run();
  testTask.post_processing();
  EXPECT_NEAR(out[0], 2.0, 1e-5);
}

TEST(average_of_vector_elements, check_int64_t) {
  // Create data
  std::vector<int64_t> in(75836, 3);
  std::vector<double> out(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<int64_t, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, true);
  testTask.pre_processing();
  testTask.run();
  testTask.post_processing();
  EXPECT_NEAR(out[0], 3.0, 1e-5);
}

TEST(average_of_vector_elements, check_float) {
  // Create data
  std::vector<float> in(1, 1.5f);
  std::vector<double> out(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  taskData->inputs_count.emplace_back(in.size());
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  taskData->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::AverageOfVectorElements<float, double> testTask(taskData);
  bool isValid = testTask.validation();
  ASSERT_EQ(isValid, true);
  testTask.pre_processing();
  testTask.run();
  testTask.post_processing();
  EXPECT_NEAR(out[0], 1.5, 1e-5);
}
