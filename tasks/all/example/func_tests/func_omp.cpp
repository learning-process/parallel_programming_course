#include <gtest/gtest.h>

#include <opencv2/opencv.hpp>
#include <random>
#include <vector>

#include "omp/example/include/ops_omp.hpp"

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

TEST(Parallel_Operations_OpenMP, Test_Sum) {
  std::vector<int> vec = GetRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "+");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();

  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Diff) {
  std::vector<int> vec = GetRandomVector(100);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Diff_2) {
  std::vector<int> vec = GetRandomVector(10);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "-");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Mult) {
  std::vector<int> vec = GetRandomVector(10);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "*");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "*");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Mult_2) {
  std::vector<int> vec = GetRandomVector(5);
  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "*");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "*");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}

TEST(Parallel_Operations_OpenMP, Test_Mult_2_File) {
  cv::Mat img = cv::imread(ppc::core::GetAbsolutePath("all/example/data/pic_omp.jpg"));
  EXPECT_EQ(static_cast<int>(sqrt(img.rows) * sqrt(img.rows)), img.cols);
  const int count_size_vector = img.rows + img.cols;
  std::vector<int> vec = GetRandomVector(count_size_vector);

  // Create data
  std::vector<int> ref_res(1, 0);

  // Create task_data
  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_seq->inputs_count.emplace_back(vec.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(ref_res.data()));
  task_data_seq->outputs_count.emplace_back(ref_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskSequential test_omp_task_sequential(task_data_seq, "*");
  ASSERT_EQ(test_omp_task_sequential.Validation(), true);
  test_omp_task_sequential.PreProcessing();
  test_omp_task_sequential.Run();
  test_omp_task_sequential.PostProcessing();

  // Create data
  std::vector<int> par_res(1, 0);

  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data_par->inputs_count.emplace_back(vec.size());
  task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t *>(par_res.data()));
  task_data_par->outputs_count.emplace_back(par_res.size());

  // Create Task
  nesterov_a_test_task_omp::TestOMPTaskParallel test_omp_task_parallel(task_data_par, "*");
  ASSERT_EQ(test_omp_task_parallel.Validation(), true);
  test_omp_task_parallel.PreProcessing();
  test_omp_task_parallel.Run();
  test_omp_task_parallel.PostProcessing();
  ASSERT_EQ(ref_res[0], par_res[0]);
}
