#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/task/func_tests/test_task.hpp"

TEST(task_tests, check_int32_t) {
  // Create data
  std::vector<int32_t> in(20, 1);

  // Create and check Task
  ppc::test::task::TestTask<int32_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.Get()), in.size());
}

TEST(task_tests, check_int32_t_slow) {
  // Create data
  std::vector<int32_t> in(20, 1);

  // Create and check Task
  ppc::test::task::FakeSlowTask<int32_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  ASSERT_ANY_THROW(test_task.PostProcessing());
}

TEST(task_tests, check_validate_func) {
  // Create data
  std::vector<int32_t> in;

  // Create and check Task
  ppc::test::task::TestTask<int32_t> test_task(in);
  bool is_valid = test_task.Validation();

  // Check Result
  ASSERT_EQ(is_valid, false);
}

TEST(task_tests, check_double) {
  // Create data
  std::vector<double> in(20, 1);

  // Create and check Task
  ppc::test::task::TestTask<double> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  EXPECT_NEAR(test_task.Get(), static_cast<double>(in.size()), 1e-6);
}

TEST(task_tests, check_uint8_t) {
  // Create data
  std::vector<uint8_t> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<uint8_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.Get()), in.size());
}

TEST(task_tests, check_int64_t) {
  // Create data
  std::vector<int64_t> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<int64_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.Get()), in.size());
}

TEST(task_tests, check_float) {
  // Create data
  std::vector<float> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<float> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  EXPECT_NEAR(test_task.Get(), in.size(), 1e-3);
}

TEST(task_tests, check_wrong_order) {
  // Create data
  std::vector<float> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<float> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  ASSERT_ANY_THROW(test_task.PostProcessing());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
