#include <gtest/gtest.h>

#include <opencv2/opencv.hpp>
#include <random>
#include <thread>
#include <vector>

#include "all/example/include/ops_stl.hpp"
#include "core/util/util.hpp"

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

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Sum_2) {
  auto nthreads = std::thread::hardware_concurrency() * 11;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Sum_3) {
  auto nthreads = std::thread::hardware_concurrency() * 13;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Diff) {
  auto nthreads = std::thread::hardware_concurrency() * 14;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Diff_2) {
  auto nthreads = std::thread::hardware_concurrency() * 15;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_STL_Threads, Test_Diff_2_File) {
  cv::Mat img = cv::imread(ppc::util::GetAbsolutePath("all/example/data/pic_stl.jpg"));
  EXPECT_EQ(static_cast<int>(sqrt(img.cols / 2) * sqrt(img.cols / 2)), img.rows);
  const int count_size_vector = static_cast<int>(sqrt(sqrt(img.rows)));

  auto nthreads = std::thread::hardware_concurrency() * count_size_vector;
  std::vector<int> vec = GetRandomVector(static_cast<int>(nthreads));
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskSequential test_stl_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_stl_task_sequential.Validation(), true);
  test_stl_task_sequential.PreProcessing();
  test_stl_task_sequential.Run();
  test_stl_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_stl::TestSTLTaskParallel test_stl_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_stl_task_parallel.Validation(), true);
  test_stl_task_parallel.PreProcessing();
  test_stl_task_parallel.Run();
  test_stl_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}
