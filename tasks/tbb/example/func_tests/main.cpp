// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>

#include <vector>

#include "tbb/example/include/ops_tbb.hpp"

TEST(Parallel_Operations_TBB, Test_Sum) {
  std::vector<int> vec = nesterov_a_test_task_tbb::getRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskSequential testTbbTaskSequential(taskDataSeq, "+");
  ASSERT_EQ(testTbbTaskSequential.validation(), true);
  testTbbTaskSequential.pre_processing();
  testTbbTaskSequential.run();
  testTbbTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskParallel testTbbTaskParallel(taskDataPar, "+");
  ASSERT_EQ(testTbbTaskParallel.validation(), true);
  testTbbTaskParallel.pre_processing();
  testTbbTaskParallel.run();
  testTbbTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_TBB, Test_Diff) {
  std::vector<int> vec = nesterov_a_test_task_tbb::getRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskSequential testTbbTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testTbbTaskSequential.validation(), true);
  testTbbTaskSequential.pre_processing();
  testTbbTaskSequential.run();
  testTbbTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskParallel testTbbTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testTbbTaskParallel.validation(), true);
  testTbbTaskParallel.pre_processing();
  testTbbTaskParallel.run();
  testTbbTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_TBB, Test_Diff_2) {
  std::vector<int> vec = nesterov_a_test_task_tbb::getRandomVector(50);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskSequential testTbbTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testTbbTaskSequential.validation(), true);
  testTbbTaskSequential.pre_processing();
  testTbbTaskSequential.run();
  testTbbTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskParallel testTbbTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testTbbTaskParallel.validation(), true);
  testTbbTaskParallel.pre_processing();
  testTbbTaskParallel.run();
  testTbbTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_TBB, Test_Mult) {
  std::vector<int> vec = nesterov_a_test_task_tbb::getRandomVector(10);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskSequential testTbbTaskSequential(taskDataSeq, "*");
  ASSERT_EQ(testTbbTaskSequential.validation(), true);
  testTbbTaskSequential.pre_processing();
  testTbbTaskSequential.run();
  testTbbTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskParallel testTbbTaskParallel(taskDataPar, "*");
  ASSERT_EQ(testTbbTaskParallel.validation(), true);
  testTbbTaskParallel.pre_processing();
  testTbbTaskParallel.run();
  testTbbTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_TBB, Test_Mult_2) {
  std::vector<int> vec = nesterov_a_test_task_tbb::getRandomVector(5);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskSequential testTbbTaskSequential(taskDataSeq, "*");
  ASSERT_EQ(testTbbTaskSequential.validation(), true);
  testTbbTaskSequential.pre_processing();
  testTbbTaskSequential.run();
  testTbbTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_tbb::TestTBBTaskParallel testTbbTaskParallel(taskDataPar, "*");
  ASSERT_EQ(testTbbTaskParallel.validation(), true);
  testTbbTaskParallel.pre_processing();
  testTbbTaskParallel.run();
  testTbbTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
