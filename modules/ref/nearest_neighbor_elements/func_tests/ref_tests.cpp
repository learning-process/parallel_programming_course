#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/task/include/task.hpp"
#include "ref/nearest_neighbor_elements/include/ref_task.hpp"

TEST(nearest_neighbor_elements, check_int32_t) {
  // Create data
  std::vector<int32_t> in(1256, 1);
  std::vector<int32_t> out(2, 0);
  std::vector<uint64_t> out_index(2, 0);
  for (size_t i = 0; i < in.size(); i++) {
    in[i] = static_cast<int32_t>(2 * i);
  }
  in[234] = 0;
  in[235] = 1;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::NearestNeighborElements<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_EQ(out[0], 0);
  EXPECT_EQ(out[1], 1);
  EXPECT_EQ(out_index[0], 234ULL);
  EXPECT_EQ(out_index[1], 235ULL);
}

TEST(nearest_neighbor_elements, check_validate_func) {
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
  ppc::reference::NearestNeighborElements<int32_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, false);
}

TEST(nearest_neighbor_elements, check_double) {
  // Create data
  std::vector<double> in(25680, 1);
  std::vector<double> out(2, 0);
  std::vector<uint64_t> out_index(2, 0);
  for (size_t i = 0; i < in.size(); i++) {
    in[i] = static_cast<double>(2 * i);
  }
  in[189] = 0.1;
  in[190] = 0.9;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::NearestNeighborElements<double, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], 0.1, 1e-6);
  EXPECT_NEAR(out[1], 0.9, 1e-6);
  EXPECT_EQ(out_index[0], 189ULL);
  EXPECT_EQ(out_index[1], 190ULL);
}

TEST(nearest_neighbor_elements, check_int8_t) {
  // Create data
  std::vector<int8_t> in(250, -1);
  std::vector<int8_t> out(2, 0);
  std::vector<uint64_t> out_index(2, 0);
  for (size_t i = 0; i < in.size(); i++) {
    if (i % 2 == 0) {
      in[i] = -50;
    } else {
      in[i] = 50;
    }
  }
  in[5] = 8;
  in[6] = -8;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::NearestNeighborElements<int8_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_EQ(out[0], 8);
  EXPECT_EQ(out[1], -8);
  EXPECT_EQ(out_index[0], 5ULL);
  EXPECT_EQ(out_index[1], 6ULL);
}

TEST(nearest_neighbor_elements, check_int64_t) {
  // Create data
  std::vector<int64_t> in(75836, 1);
  std::vector<int64_t> out(2, 0);
  std::vector<uint64_t> out_index(2, 0);
  for (size_t i = 0; i < in.size(); i++) {
    if (i % 3 == 0) {
      in[i] = 10;
    } else if (i % 3 == 1) {
      in[i] = 30;
    } else {  // (i % 3 == 2)
      in[i] = 70;
    }
  }
  in[20] = -100;
  in[21] = -119;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::NearestNeighborElements<int64_t, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_EQ(out[0], -100);
  EXPECT_EQ(out[1], -119);
  EXPECT_EQ(out_index[0], 20ULL);
  EXPECT_EQ(out_index[1], 21ULL);
}

TEST(nearest_neighbor_elements, check_float) {
  // Create data
  std::vector<float> in(20, 1.F);
  std::vector<float> out(2, 0.F);
  std::vector<uint64_t> out_index(2, 0);
  for (size_t i = 0; i < in.size(); i++) {
    in[i] += static_cast<float>(i + 1) * 2.5F;
  }
  in[0] = 0.001F;
  in[1] = 0.0025F;

  // Create task_data
  auto task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
  task_data->inputs_count.emplace_back(in.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out_index.data()));
  task_data->outputs_count.emplace_back(out_index.size());

  // Create Task
  ppc::reference::NearestNeighborElements<float, uint64_t> test_task(task_data);
  bool is_valid = test_task.Validation();
  EXPECT_EQ(is_valid, true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(out[0], 0.001F, 1e-4F);
  EXPECT_NEAR(out[1], 0.0025F, 1e-4F);
  EXPECT_EQ(out_index[0], 0ULL);
  EXPECT_EQ(out_index[1], 1ULL);
}
