#include <gtest/gtest.h>

#include <random>
#include <thread>
#include <vector>

#include "stl/example/include/ops_stl.hpp"

namespace {
std::vector<int> GetRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 100 - 50;
  }
  return vec;
}
}  // namespace

TEST(Parallel_Operations_STL_Threads, Test_Sum) {
  auto nthreads = std::thread::hardware_concurrency() * 10;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.validation(), true);
  test_stl_task_sequential.pre_processing();
  test_stl_task_sequential.run();
  test_stl_task_sequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.validation(), true);
  test_stl_task_parallel.pre_processing();
  test_stl_task_parallel.run();
  test_stl_task_parallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Sum_2) {
  auto nthreads = std::thread::hardware_concurrency() * 11;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.validation(), true);
  test_stl_task_sequential.pre_processing();
  test_stl_task_sequential.run();
  test_stl_task_sequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.validation(), true);
  test_stl_task_parallel.pre_processing();
  test_stl_task_parallel.run();
  test_stl_task_parallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Sum_3) {
  auto nthreads = std::thread::hardware_concurrency() * 13;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.validation(), true);
  test_stl_task_sequential.pre_processing();
  test_stl_task_sequential.run();
  test_stl_task_sequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.validation(), true);
  test_stl_task_parallel.pre_processing();
  test_stl_task_parallel.run();
  test_stl_task_parallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Diff) {
  auto nthreads = std::thread::hardware_concurrency() * 14;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_stl_task_sequential.validation(), true);
  test_stl_task_sequential.pre_processing();
  test_stl_task_sequential.run();
  test_stl_task_sequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_stl_task_parallel.validation(), true);
  test_stl_task_parallel.pre_processing();
  test_stl_task_parallel.run();
  test_stl_task_parallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Diff_2) {
  auto nthreads = std::thread::hardware_concurrency() * 15;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create TaskData
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_stl_task_sequential.validation(), true);
  test_stl_task_sequential.pre_processing();
  test_stl_task_sequential.run();
  test_stl_task_sequential.post_processing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_stl_task_parallel.validation(), true);
  test_stl_task_parallel.pre_processing();
  test_stl_task_parallel.run();
  test_stl_task_parallel.post_processing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}
