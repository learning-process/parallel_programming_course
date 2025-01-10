#include <gtest/gtest.h>

#include <vector>

#include "seq/example1/include/ops_seq.hpp"

TEST(Sequential1, Test_Sum_10) {
  const int count = 10;

  // Create data
  std::vector<int> in(1, count);
  std::vector<int> out(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_seq->inputs_count.emplace_back(in.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_seq->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_seq1::TestTaskSequential test_task_sequential(task_data_seq);
  ASSERT_EQ(test_task_sequential.validation(), true);
  test_task_sequential.pre_processing();
  test_task_sequential.run();
  test_task_sequential.post_processing();
  ASSERT_EQ(count, out[0]);
}

TEST(Sequential1, Test_Sum_20) {
  const int count = 20;

  // Create data
  std::vector<int> in(1, count);
  std::vector<int> out(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_seq->inputs_count.emplace_back(in.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_seq->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_seq1::TestTaskSequential test_task_sequential(task_data_seq);
  ASSERT_EQ(test_task_sequential.validation(), true);
  test_task_sequential.pre_processing();
  test_task_sequential.run();
  test_task_sequential.post_processing();
  ASSERT_EQ(count, out[0]);
}

TEST(Sequential1, Test_Sum_50) {
  const int count = 50;

  // Create data
  std::vector<int> in(1, count);
  std::vector<int> out(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_seq->inputs_count.emplace_back(in.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_seq->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_seq1::TestTaskSequential test_task_sequential(task_data_seq);
  ASSERT_EQ(test_task_sequential.validation(), true);
  test_task_sequential.pre_processing();
  test_task_sequential.run();
  test_task_sequential.post_processing();
  ASSERT_EQ(count, out[0]);
}

TEST(Sequential1, Test_Sum_70) {
  const int count = 70;

  // Create data
  std::vector<int> in(1, count);
  std::vector<int> out(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_seq->inputs_count.emplace_back(in.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_seq->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_seq1::TestTaskSequential test_task_sequential(task_data_seq);
  ASSERT_EQ(test_task_sequential.validation(), true);
  test_task_sequential.pre_processing();
  test_task_sequential.run();
  test_task_sequential.post_processing();
  ASSERT_EQ(count, out[0]);
}

TEST(Sequential1, Test_Sum_100) {
  const int count = 100;

  // Create data
  std::vector<int> in(1, count);
  std::vector<int> out(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_seq->inputs_count.emplace_back(in.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_seq->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_seq1::TestTaskSequential test_task_sequential(task_data_seq);
  ASSERT_EQ(test_task_sequential.validation(), true);
  test_task_sequential.pre_processing();
  test_task_sequential.run();
  test_task_sequential.post_processing();
  ASSERT_EQ(count, out[0]);
}
