#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/task/tests/test_task.hpp"

TEST(task_tests, check_int32_t) {
  // Create data
  std::vector<int32_t> in(20, 1);

  // Create and check Task
  ppc::test::task::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(task_tests, check_int32_t_slow) {
  // Create data
  std::vector<int32_t> in(20, 1);

  // Create and check Task
  ppc::test::task::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
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
  ppc::test::task::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  bool is_valid = test_task.Validation();

  // Check Result
  ASSERT_EQ(is_valid, false);

  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
}

TEST(task_tests, check_double) {
  // Create data
  std::vector<double> in(20, 1);

  // Create and check Task
  ppc::test::task::TestTask<std::vector<double>, double> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  EXPECT_NEAR(test_task.GetOutput(), static_cast<double>(in.size()), 1e-6);
}

TEST(task_tests, check_uint8_t) {
  // Create data
  std::vector<uint8_t> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<std::vector<uint8_t>, uint8_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(task_tests, check_int64_t) {
  // Create data
  std::vector<int64_t> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<std::vector<int64_t>, int64_t> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(task_tests, check_float) {
  // Create data
  std::vector<float> in(20, 1);

  // Create Task
  ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  bool is_valid = test_task.Validation();
  ASSERT_EQ(is_valid, true);

  // Run Task
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();

  // Check Result
  EXPECT_NEAR(test_task.GetOutput(), in.size(), 1e-3);
}

TEST(task_tests, check_wrong_order_disabled_valgrind) {
  auto destroy_function = [] {
    // Create data
    std::vector<float> in(20, 1);

    // Create Task
    ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
    bool is_valid = test_task.Validation();
    ASSERT_EQ(is_valid, true);
    test_task.PreProcessing();
    test_task.PostProcessing();
  };
  EXPECT_DEATH_IF_SUPPORTED(destroy_function(), ".*ORDER OF FUNCTIONS IS NOT RIGHT.*");
}

TEST(task_tests, check_empty_order_disabled_valgrind) {
  auto destroy_function = [] {
    // Create data
    std::vector<float> in(20, 1);

    // Create Task
    ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  };
  EXPECT_DEATH_IF_SUPPORTED(destroy_function(), ".*ORDER OF FUNCTIONS IS NOT RIGHT.*");
}

TEST(task_tests, premature_postprocessing_no_steps) {
  auto destroy_function = [] {
    std::vector<float> in(20, 1);
    ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
    ASSERT_NO_THROW(test_task.PostProcessing());
  };
  EXPECT_DEATH_IF_SUPPORTED(destroy_function(), ".*ORDER OF FUNCTIONS IS NOT RIGHT.*");
}

TEST(task_tests, premature_postprocessing_after_preprocessing) {
  auto destroy_function = [] {
    std::vector<float> in(20, 1);
    ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
    ASSERT_NO_THROW(test_task.PreProcessing());
    ASSERT_NO_THROW(test_task.PostProcessing());
  };
  EXPECT_DEATH_IF_SUPPORTED(destroy_function(), ".*ORDER OF FUNCTIONS IS NOT RIGHT.*");
}

// Test for disabled status string
TEST(TaskTest, GetStringTaskStatus_Disabled) {
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kDisabled), "disabled");
}

// Test for enabled status string
TEST(TaskTest, GetStringTaskStatus_Enabled) {
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kEnabled), "enabled");
}

// Test for exception thrown when file cannot be opened
TEST(TaskTest, GetStringTaskType_InvalidFileThrows) {
  EXPECT_THROW({ GetStringTaskType(ppc::core::TypeOfTask::kALL, "non_existing_file.json"); }, std::runtime_error);
}

// Test unknown type case
TEST(TaskTest, GetStringTaskType_UnknownType) {
  std::string path = "settings.json";  // Provide valid settings file
  EXPECT_NO_THROW({ GetStringTaskType(ppc::core::TypeOfTask::kUnknown, path); });
}

// Death test for destructor when pipeline order is invalid
TEST(TaskTest, Destructor_InvalidPipelineOrderTerminates) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  auto test_func = [&] {
    struct BadTask : ppc::core::Task<int, int> {
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task;
  };
  ASSERT_DEATH_IF_SUPPORTED({ test_func(); }, "");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
