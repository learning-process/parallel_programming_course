#include <gtest/gtest.h>

#include <vector>

#include "core/task/func_tests/test_task.hpp"
#include "core/task/include/task.hpp"

TEST(task_tests, check_int32_t) {
  using InOutType = int32_t;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  test_task.run();
  test_task.post_process();
  EXPECT_EQ(out, static_cast<InOutType>(in.size()));
}

TEST(task_tests, check_validate_func) {
  using InOutType = int32_t;

  // Create data
  std::vector<InOutType> in(20, 1);
  std::vector<InOutType> out(2, 0);

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  EXPECT_FALSE(test_task.validate());
}

TEST(task_tests, check_double) {
  using InOutType = double;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  test_task.run();
  test_task.post_process();
  EXPECT_NEAR(out, static_cast<InOutType>(in.size()), 1e-6);
}

TEST(task_tests, check_uint8_t) {
  using InOutType = uint32_t;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  test_task.run();
  test_task.post_process();
  EXPECT_EQ(out, static_cast<InOutType>(in.size()));
}

TEST(task_tests, check_int64_t) {
  using InOutType = int64_t;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  test_task.run();
  test_task.post_process();
  EXPECT_EQ(out, static_cast<InOutType>(in.size()));
}

TEST(task_tests, check_float) {
  using InOutType = float;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  test_task.run();
  test_task.post_process();
  EXPECT_NEAR(out, static_cast<InOutType>(in.size()), 1e-3);
}

TEST(task_tests, check_wrong_order) {
  using InOutType = int32_t;

  // Create data
  std::vector<InOutType> in(20, 1);
  InOutType out;

  // Create TaskData
  ppc::test::task::TestTask<InOutType>::TaskData task_data(in, out);
  // Create Task
  ppc::test::task::TestTask<InOutType> test_task(task_data);
  ASSERT_TRUE(test_task.validate());
  test_task.pre_process();
  EXPECT_ANY_THROW(test_task.post_process());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
