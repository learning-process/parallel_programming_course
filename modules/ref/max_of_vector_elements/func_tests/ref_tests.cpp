#include <gtest/gtest.h>

#include <vector>

#include "core/task/include/task.hpp"
#include "ref/max_of_vector_elements/include/ref_task.hpp"

TEST(max_of_vector_elements, check_int32_t) {
  using InOutType = int32_t;
  using IndexType = uint64_t;

  // Create data
  std::vector<InOutType> in(1256, 1);
  std::pair<IndexType, InOutType> out;
  in[328] = 10;

  // Create TaskData
  ppc::reference::MaxOfVectorElements<InOutType, IndexType>::TaskData taskData(in, out);
  // Create Task
  ppc::reference::MaxOfVectorElements<InOutType, IndexType> testTask(taskData);
  ASSERT_TRUE(testTask.validate());
  testTask.pre_process();
  testTask.run();
  testTask.post_process();
  EXPECT_EQ(out, std::make_pair(IndexType(328), InOutType(10)));
}

TEST(max_of_vector_elements, check_double) {
  using InOutType = double;
  using IndexType = uint64_t;

  // Create data
  std::vector<InOutType> in(25680, 1);
  std::pair<IndexType, InOutType> out;
  in[2] = 1.0001;

  // Create TaskData
  ppc::reference::MaxOfVectorElements<InOutType, IndexType>::TaskData taskData(in, out);
  // Create Task
  ppc::reference::MaxOfVectorElements<InOutType, IndexType> testTask(taskData);
  ASSERT_TRUE(testTask.validate());
  testTask.pre_process();
  testTask.run();
  testTask.post_process();
  EXPECT_EQ(out.first, 2ull);
  EXPECT_NEAR(out.second, 1.0001, 1e-6);
}

TEST(max_of_vector_elements, check_uint8_t) {
  using InOutType = uint32_t;
  using IndexType = uint64_t;

  // Create data
  std::vector<InOutType> in(1256, 1);
  std::pair<IndexType, InOutType> out;
  in[328] = 10;

  // Create TaskData
  ppc::reference::MaxOfVectorElements<InOutType, IndexType>::TaskData taskData(in, out);
  // Create Task
  ppc::reference::MaxOfVectorElements<InOutType, IndexType> testTask(taskData);
  ASSERT_TRUE(testTask.validate());
  testTask.pre_process();
  testTask.run();
  testTask.post_process();
  EXPECT_EQ(out, std::make_pair(IndexType(0), InOutType(1)));
}

TEST(max_of_vector_elements, check_int64_t) {
  using InOutType = int64_t;
  using IndexType = uint64_t;

  // Create data
  std::vector<InOutType> in(75836, 1);
  std::pair<IndexType, InOutType> out;
  in[345] = 256;

  // Create TaskData
  ppc::reference::MaxOfVectorElements<InOutType, IndexType>::TaskData taskData(in, out);
  // Create Task
  ppc::reference::MaxOfVectorElements<InOutType, IndexType> testTask(taskData);
  ASSERT_TRUE(testTask.validate());
  testTask.pre_process();
  testTask.run();
  testTask.post_process();
  EXPECT_EQ(out, std::make_pair(IndexType(345), InOutType(256)));
}

TEST(max_of_vector_elements, check_float) {
  using InOutType = float;
  using IndexType = uint64_t;

  // Create data
  std::vector<InOutType> in(1, 1.f);
  std::pair<IndexType, InOutType> out;
  in[0] = 1.01f;

  // Create TaskData
  ppc::reference::MaxOfVectorElements<InOutType, IndexType>::TaskData taskData(in, out);
  // Create Task
  ppc::reference::MaxOfVectorElements<InOutType, IndexType> testTask(taskData);
  ASSERT_TRUE(testTask.validate());
  testTask.pre_process();
  testTask.run();
  testTask.post_process();
  EXPECT_EQ(out.first, 2ull);
  EXPECT_NEAR(out.second, 1.01f, 1e-6f);
}
