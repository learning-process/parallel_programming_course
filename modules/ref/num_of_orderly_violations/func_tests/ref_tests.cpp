#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"
#include "ref/num_of_orderly_violations/include/ref_task.hpp"

TEST(num_of_orderly_violations, check_int32_t) {
  // Create data
  std::vector<int32_t> in(1256, 1);
  std::vector<uint64_t> out(1, 0);
  for (size_t i = 0; i < in.size(); i++) {
    if (i % 2 == 0) {
      in[i] *= -1;
    }
  }

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::NumOfOrderlyViolations<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], (in.size() / 2) - 1);
}

TEST(num_of_orderly_violations, check_validate_func) {
  // Create data
  std::vector<int32_t> in(125, 1);
  std::vector<int32_t> out(2, 0);
  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  // Create Task
  ppc::reference::NumOfOrderlyViolations<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, false);
}

TEST(num_of_orderly_violations, check_double) {
  // Create data
  std::vector<double> in(25680, 1);
  std::vector<uint64_t> out(1, 0);
  in[1] = -1;
  in[200] = -1;
  in[456] = -1;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::NumOfOrderlyViolations<double, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], 3ULL);
}

TEST(num_of_orderly_violations, check_int8_t) {
  // Create data
  std::vector<int8_t> in(255, -1);
  std::vector<uint64_t> out(1, 0);

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::NumOfOrderlyViolations<int8_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], 0ULL);
}

TEST(num_of_orderly_violations, check_int64_t) {
  // Create data
  std::vector<int64_t> in(75836, 1);
  std::vector<uint64_t> out(1, 0);
  for (size_t i = 500; i < in.size() - 600; i++) {
    in[i] *= -1;
  }

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::NumOfOrderlyViolations<int64_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], 1ULL);
}

TEST(num_of_orderly_violations, check_float) {
  // Create data
  std::vector<float> in(3, 1.F);
  std::vector<uint64_t> out(1, 0);
  in[1] = -1.0001F;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  // Create Task
  ppc::reference::NumOfOrderlyViolations<float, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(out[0], 1ULL);
}
