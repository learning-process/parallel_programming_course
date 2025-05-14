#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"
#include "stl/example/include/ops_stl.hpp"

TEST(nesterov_a_test_task_stl, test_matmul_50) {
  constexpr size_t kCount = 50;

  // Create data
  std::vector<int> in(kCount * kCount, 0);

  for (size_t i = 0; i < kCount; i++) {
    in[(i * kCount) + i] = 1;
  }

  // Create Task
  nesterov_a_test_task_stl::TestTaskSTL test_task_stl(in);
  ASSERT_EQ(test_task_stl.Validation(), true);
  test_task_stl.PreProcessing();
  test_task_stl.Run();
  test_task_stl.PostProcessing();
  EXPECT_EQ(in, test_task_stl.Get());
}

TEST(nesterov_a_test_task_stl, test_matmul_100_from_file) {
  std::string line;
  std::ifstream test_file(ppc::util::GetAbsolutePath("stl/example/data/test.txt"));
  if (test_file.is_open()) {
    getline(test_file, line);
  }
  test_file.close();

  const size_t count = std::stoi(line);

  // Create data
  std::vector<int> in(count * count, 0);

  for (size_t i = 0; i < count; i++) {
    in[(i * count) + i] = 1;
  }

  // Create Task
  nesterov_a_test_task_stl::TestTaskSTL test_task_stl(in);
  ASSERT_EQ(test_task_stl.Validation(), true);
  test_task_stl.PreProcessing();
  test_task_stl.Run();
  test_task_stl.PostProcessing();
  EXPECT_EQ(in, test_task_stl.Get());
}
