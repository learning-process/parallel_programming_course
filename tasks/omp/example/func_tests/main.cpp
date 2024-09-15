// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>

#include <vector>

#include "omp/example/include/ops_omp.hpp"

TEST(Parallel_Operations_OpenMP, Test_Sum) {
  std::vector<int> vec = nesterov_a_test_task_omp::getRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential testOmpTaskSequential(taskDataSeq, "+");
  ASSERT_EQ(testOmpTaskSequential.validation(), true);
  testOmpTaskSequential.pre_processing();
  testOmpTaskSequential.run();
  testOmpTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel testOmpTaskParallel(taskDataPar, "+");
  ASSERT_EQ(testOmpTaskParallel.validation(), true);
  testOmpTaskParallel.pre_processing();
  testOmpTaskParallel.run();
  testOmpTaskParallel.post_processing();

  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Diff) {
  std::vector<int> vec = nesterov_a_test_task_omp::getRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential testOmpTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testOmpTaskSequential.validation(), true);
  testOmpTaskSequential.pre_processing();
  testOmpTaskSequential.run();
  testOmpTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel testOmpTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testOmpTaskParallel.validation(), true);
  testOmpTaskParallel.pre_processing();
  testOmpTaskParallel.run();
  testOmpTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Diff_2) {
  std::vector<int> vec = nesterov_a_test_task_omp::getRandomVector(10);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential testOmpTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testOmpTaskSequential.validation(), true);
  testOmpTaskSequential.pre_processing();
  testOmpTaskSequential.run();
  testOmpTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel testOmpTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testOmpTaskParallel.validation(), true);
  testOmpTaskParallel.pre_processing();
  testOmpTaskParallel.run();
  testOmpTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Mult) {
  std::vector<int> vec = nesterov_a_test_task_omp::getRandomVector(10);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential testOmpTaskSequential(taskDataSeq, "*");
  ASSERT_EQ(testOmpTaskSequential.validation(), true);
  testOmpTaskSequential.pre_processing();
  testOmpTaskSequential.run();
  testOmpTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel testOmpTaskParallel(taskDataPar, "*");
  ASSERT_EQ(testOmpTaskParallel.validation(), true);
  testOmpTaskParallel.pre_processing();
  testOmpTaskParallel.run();
  testOmpTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Mult_2) {
  std::vector<int> vec = nesterov_a_test_task_omp::getRandomVector(5);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential testOmpTaskSequential(taskDataSeq, "*");
  ASSERT_EQ(testOmpTaskSequential.validation(), true);
  testOmpTaskSequential.pre_processing();
  testOmpTaskSequential.run();
  testOmpTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel testOmpTaskParallel(taskDataPar, "*");
  ASSERT_EQ(testOmpTaskParallel.validation(), true);
  testOmpTaskParallel.pre_processing();
  testOmpTaskParallel.run();
  testOmpTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
