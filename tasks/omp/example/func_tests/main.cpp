#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include "core/util/util.hpp"
#include "omp/example/include/ops_omp.hpp"

TEST(nesterov_a_test_task_omp, test_matmul_50) {
  const size_t count = 50;

  // Create data
  std::vector<int> in(count * count, 0);
  std::vector<int> out(count * count, 0);

  for (size_t i = 0; i < count; i++) {
    in[(i * count) + i] = 1;
  }

  // Create task_data
  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_omp->inputs_count.emplace_back(in.size());
  task_data_omp->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_omp->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_omp::TestTaskOpenMP test_task_omp(task_data_omp);
  ASSERT_EQ(test_task_omp.Validation(), true);
  test_task_omp.PreProcessing();
  test_task_omp.Run();
  test_task_omp.PostProcessing();
  EXPECT_EQ(in, out);
}

TEST(nesterov_a_test_task_omp, test_matmul_100_from_file) {
  std::string line;
  std::ifstream test_file(ppc::util::GetAbsolutePath("omp/example/data/test.txt"));
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
  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_omp->inputs_count.emplace_back(in.size());
  task_data_omp->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_omp->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_omp::TestTaskOpenMP test_task_omp(task_data_omp);
  ASSERT_EQ(test_task_omp.Validation(), true);
  test_task_omp.PreProcessing();
  test_task_omp.Run();
  test_task_omp.PostProcessing();
  EXPECT_EQ(in, out);
}
