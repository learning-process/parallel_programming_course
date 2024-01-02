// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "./ops_std.h"

TEST(Parallel_Operations_Std_Threads, Test_Sum) {
  auto nthreads = std::thread::hardware_concurrency() * 10;
  std::vector<int> vec = getRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  TestSTDTaskSequential testStdTaskSequential(taskDataSeq, "+");
  ASSERT_EQ(testStdTaskSequential.validation(), true);
  testStdTaskSequential.pre_processing();
  testStdTaskSequential.run();
  testStdTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  TestSTDTaskParallel testStdTaskParallel(taskDataPar, "+");
  ASSERT_EQ(testStdTaskParallel.validation(), true);
  testStdTaskParallel.pre_processing();
  testStdTaskParallel.run();
  testStdTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_2) {
  auto nthreads = std::thread::hardware_concurrency() * 11;
  std::vector<int> vec = getRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  TestSTDTaskSequential testStdTaskSequential(taskDataSeq, "+");
  ASSERT_EQ(testStdTaskSequential.validation(), true);
  testStdTaskSequential.pre_processing();
  testStdTaskSequential.run();
  testStdTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  TestSTDTaskParallel testStdTaskParallel(taskDataPar, "+");
  ASSERT_EQ(testStdTaskParallel.validation(), true);
  testStdTaskParallel.pre_processing();
  testStdTaskParallel.run();
  testStdTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_3) {
  auto nthreads = std::thread::hardware_concurrency() * 13;
  std::vector<int> vec = getRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  TestSTDTaskSequential testStdTaskSequential(taskDataSeq, "+");
  ASSERT_EQ(testStdTaskSequential.validation(), true);
  testStdTaskSequential.pre_processing();
  testStdTaskSequential.run();
  testStdTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  TestSTDTaskParallel testStdTaskParallel(taskDataPar, "+");
  ASSERT_EQ(testStdTaskParallel.validation(), true);
  testStdTaskParallel.pre_processing();
  testStdTaskParallel.run();
  testStdTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff) {
  auto nthreads = std::thread::hardware_concurrency() * 14;
  std::vector<int> vec = getRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  TestSTDTaskSequential testStdTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testStdTaskSequential.validation(), true);
  testStdTaskSequential.pre_processing();
  testStdTaskSequential.run();
  testStdTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  TestSTDTaskParallel testStdTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testStdTaskParallel.validation(), true);
  testStdTaskParallel.pre_processing();
  testStdTaskParallel.run();
  testStdTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff_2) {
  auto nthreads = std::thread::hardware_concurrency() * 15;
  std::vector<int> vec = getRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataSeq->inputs_count.emplace_back(vec.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  taskDataSeq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  TestSTDTaskSequential testStdTaskSequential(taskDataSeq, "-");
  ASSERT_EQ(testStdTaskSequential.validation(), true);
  testStdTaskSequential.pre_processing();
  testStdTaskSequential.run();
  testStdTaskSequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  taskDataPar->inputs_count.emplace_back(vec.size());
  taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  taskDataPar->outputs_count.emplace_back(par_res.size());

  // Create Task
  TestSTDTaskParallel testStdTaskParallel(taskDataPar, "-");
  ASSERT_EQ(testStdTaskParallel.validation(), true);
  testStdTaskParallel.pre_processing();
  testStdTaskParallel.run();
  testStdTaskParallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
