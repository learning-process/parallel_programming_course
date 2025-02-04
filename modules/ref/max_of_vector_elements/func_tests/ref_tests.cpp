#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"
#include "ref/max_of_vector_elements/include/ref_task.hpp"

TEST(max_of_vector_elements, check_int32_t) {
  // Create data
  std::vector<int32_t> in(1256, 1);
  std::vector<int32_t> out(1, 0);
  std::vector<uint64_t> out_index(1, 0);
  in[328] = 10;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], 10);
  ASSERT_EQ(out_index[0], 328ULL);
}

TEST(max_of_vector_elements, check_validate_func_1) {
  // Create data
  std::vector<int32_t> in(125, 1);
  std::vector<int32_t> out(2, 0);
  std::vector<uint64_t> out_index(1, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, false);
}

TEST(max_of_vector_elements, check_validate_func_2) {
  // Create data
  std::vector<int32_t> in(125, 1);
  std::vector<int32_t> out(1, 0);
  std::vector<uint64_t> out_index(2, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, false);
}

TEST(max_of_vector_elements, check_double) {
  // Create data
  std::vector<double> in(25680, 1);
  std::vector<double> out(1, 0);
  std::vector<uint64_t> out_index(1, 0);
  in[2] = 1.0001;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<double, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], 1.0001, 1e-6);
  ASSERT_EQ(out_index[0], 2ULL);
}

TEST(max_of_vector_elements, check_uint8_t) {
  // Create data
  std::vector<uint8_t> in(255, 1);
  std::vector<uint8_t> out(1, 0);
  std::vector<uint64_t> out_index(1, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<uint8_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_EQ(out[0], 1);
  ASSERT_EQ(out_index[0], 0ULL);
}

TEST(max_of_vector_elements, check_int64_t) {
  // Create data
  std::vector<int64_t> in(75836, 1);
  std::vector<int64_t> out(1, 0);
  std::vector<uint64_t> out_index(1, 0);
  in[345] = 256;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<int64_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_EQ(out[0], 256LL);
  ASSERT_EQ(out_index[0], 345ULL);
}

TEST(max_of_vector_elements, check_float) {
  // Create data
  std::vector<float> in(1, 1.F);
  std::vector<float> out(1, 0.F);
  std::vector<uint64_t> out_index(1, 0);
  in[0] = 1.01F;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::MaxOfVectorElements<float, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], 1.01F, 1e-6F);
  ASSERT_EQ(out_index[0], 0ULL);
}
