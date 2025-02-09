#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"
#include "tbb/example/include/ops_tbb.hpp"

TEST(nesterov_a_test_task_tbb, test_matmul_50) {
  constexpr size_t kCount = 50;

  // Create data
  std::vector<int> in(kCount * kCount, 0);
  std::vector<int> out(kCount * kCount, 0);

  for (size_t i = 0; i < kCount; i++) {
    in[(i * kCount) + i] = 1;
  }

  // Create task_data
  auto task_data_tbb = std::make_shared<ppc::core::TaskData>();
  task_data_tbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_tbb->inputs_count.emplace_back(in.size());
  task_data_tbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_tbb->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTaskTBB test_task_tbb(task_data_tbb);
  ASSERT_EQ(test_task_tbb.Validation(), true);
  test_task_tbb.PreProcessing();
  test_task_tbb.Run();
  test_task_tbb.PostProcessing();
  EXPECT_EQ(in, out);
}

TEST(nesterov_a_test_task_tbb, test_matmul_100_from_file) {
  std::string line;
  std::ifstream test_file(ppc::util::GetAbsolutePath("tbb/example/data/test.txt"));
  if (test_file.is_open()) {
    getline(test_file, line);
  }
  test_file.close();

  const size_t count = std::stoi(line);

  // Create data
  std::vector<int> in(count * count, 0);
  std::vector<int> out(count * count, 0);

  for (size_t i = 0; i < count; i++) {
    in[(i * count) + i] = 1;
  }

  // Create task_data
  auto task_data_tbb = std::make_shared<ppc::core::TaskData>();
  task_data_tbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_tbb->inputs_count.emplace_back(in.size());
  task_data_tbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_tbb->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTaskTBB test_task_tbb(task_data_tbb);
  ASSERT_EQ(test_task_tbb.Validation(), true);
  test_task_tbb.PreProcessing();
  test_task_tbb.Run();
  test_task_tbb.PostProcessing();
  EXPECT_EQ(in, out);
}
