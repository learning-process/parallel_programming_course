#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"
#include "ref/vector_dot_product/include/ref_task.hpp"

TEST(vector_dot_product, check_int32_t) {
  // Create data
  const uint64_t count_data = 1256;
  std::vector<int32_t> in1(count_data, 1);
  std::vector<int32_t> in2(count_data, 1);
  std::vector<int32_t> out(1, 0);
  for (size_t i = 0; i < count_data; i++) {
    in1[i] = static_cast<int32_t>(i + 1);
    in2[i] = static_cast<int32_t>(i + 1);
  }

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<int32_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(static_cast<uint64_t>(out[0]), (count_data * (count_data + 1) * (2 * count_data + 1)) / 6);
}

TEST(vector_dot_product, check_validate_func) {
  // Create data
  std::vector<int32_t> in1(125, 1);
  std::vector<int32_t> in2(125, 1);
  std::vector<int32_t> out(2, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<int32_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, false);
}

TEST(vector_dot_product, check_validate_func_2) {
  // Create data
  std::vector<int32_t> in1(125, 1);
  std::vector<int32_t> in2(124, 1);
  std::vector<int32_t> out(1, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<int32_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, false);
}

TEST(vector_dot_product, check_double) {
  // Create data
  std::vector<double> in1(25680, 1.1);
  std::vector<double> in2(25680, 1.3);
  std::vector<double> out(1, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<double> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], in1.size() * 1.1 * 1.3, 1e-6);
}

TEST(vector_dot_product, check_int8_t) {
  // Create data
  std::vector<int8_t> in1(126, 1);
  std::vector<int8_t> in2(126, 1);
  std::vector<int8_t> out(1, 0);
  for (size_t i = 0; i < in1.size(); ++i) {
    in1[i] = in2[i] = (i % 2 == 0) ? 1 : -1;
  }

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<int8_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(static_cast<size_t>(out[0]), in1.size());
}

TEST(vector_dot_product, check_int64_t) {
  // Create data
  const uint64_t count_data = 297;
  std::vector<int64_t> in1(count_data, 1);
  std::vector<int64_t> in2(count_data, 1);
  std::vector<int64_t> out(1, 0);
  for (uint64_t i = 0; i < count_data; i++) {
    in1[i] = static_cast<int64_t>(i + 1);
    in2[i] = static_cast<int64_t>((i + 1) * (i + 1));
  }

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<int64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], static_cast<int64_t>(count_data * count_data * (count_data + 1) * (count_data + 1)) / 4);
}

TEST(vector_dot_product, check_float) {
  // Create data
  std::vector<float> in1(1, -1.3F);
  std::vector<float> in2(1, 1.2F);
  std::vector<float> out(1, 0.F);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in1.data()));
  task_data->inputs_count.emplace_back(in1.size());
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in2.data()));
  task_data->inputs_count.emplace_back(in2.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::VectorDotProduct<float> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], in1.size() * (-1.3F) * 1.2F, 1e-3F);
}
