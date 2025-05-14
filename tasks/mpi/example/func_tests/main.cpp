#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"
#include "mpi/example/include/ops_mpi.hpp"

TEST(nesterov_a_test_task_mpi, test_matmul_50) {
  constexpr size_t kCount = 50;

  // Create data
  std::vector<int> in(kCount * kCount, 0);

  for (size_t i = 0; i < kCount; i++) {
    in[(i * kCount) + i] = 1;
  }

  // Create Task
  nesterov_a_test_task_mpi::TestTaskMPI test_task_mpi(in);
  ASSERT_EQ(test_task_mpi.Validation(), true);
  test_task_mpi.PreProcessing();
  test_task_mpi.Run();
  test_task_mpi.PostProcessing();

  EXPECT_EQ(in, test_task_mpi.Get());
}

TEST(nesterov_a_test_task_mpi, test_matmul_100_from_file) {
  std::string line;
  std::ifstream test_file(ppc::util::GetAbsolutePath("mpi/example/data/test.txt"));
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
  nesterov_a_test_task_mpi::TestTaskMPI test_task_mpi(in);
  ASSERT_EQ(test_task_mpi.Validation(), true);
  test_task_mpi.PreProcessing();
  test_task_mpi.Run();
  test_task_mpi.PostProcessing();

  EXPECT_EQ(in, test_task_mpi.Get());
}
