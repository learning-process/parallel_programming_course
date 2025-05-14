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

class NesterovATestTaskSTL : public ::testing::TestWithParam<double> {
 protected:
  void SetUp() override {
    std::ifstream test_file(ppc::util::GetAbsolutePath("stl/example/data/test.txt"));
    ASSERT_TRUE(test_file.is_open()) << "Failed to open input file";
    std::string line;
    std::getline(test_file, line);
    test_file.close();
    base_count = std::stoi(line);
  }

  [[nodiscard]] size_t GetCount() const { return static_cast<size_t>(base_count * GetParam()); }

  int base_count = 0;
};

TEST_P(NesterovATestTaskSTL, MatmulFromFile) {
  const size_t count = GetCount();

  std::vector<int> in(count * count, 0);
  for (size_t i = 0; i < count; ++i) {
    in[(i * count) + i] = 1;
  }

  nesterov_a_test_task_stl::TestTaskSTL test_task_stl(in);
  ASSERT_TRUE(test_task_stl.Validation());
  test_task_stl.PreProcessing();
  test_task_stl.Run();
  test_task_stl.PostProcessing();

  EXPECT_EQ(in, test_task_stl.Get());
}

INSTANTIATE_TEST_SUITE_P(FileMatrixTestsSTL, NesterovATestTaskSTL, ::testing::Values(0.5, 1.0));
