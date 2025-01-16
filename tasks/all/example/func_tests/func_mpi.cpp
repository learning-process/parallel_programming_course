#include <gtest/gtest.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <fstream>
#include <random>
#include <vector>

#include "mpi/example/include/ops_mpi.hpp"

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

TEST(Parallel_Operations_MPI, Test_Sum) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_sum(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    const int count_size_vector = 120;
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_sum.data()));
    task_data_par->outputs_count.emplace_back(global_sum.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "+");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_sum(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_sum.data()));
    task_data_seq->outputs_count.emplace_back(reference_sum.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "+");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_sum[0], global_sum[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Diff) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_diff(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    const int count_size_vector = 240;
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_diff.data()));
    task_data_par->outputs_count.emplace_back(global_diff.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_diff(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_diff.data()));
    task_data_seq->outputs_count.emplace_back(reference_diff.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "-");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_diff[0], global_diff[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Diff_2) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_diff(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    const int count_size_vector = 120;
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_diff.data()));
    task_data_par->outputs_count.emplace_back(global_diff.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "-");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_diff(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_diff.data()));
    task_data_seq->outputs_count.emplace_back(reference_diff.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "-");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_diff[0], global_diff[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Max) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_max(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    const int count_size_vector = 240;
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_max.data()));
    task_data_par->outputs_count.emplace_back(global_max.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "max");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_max(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_max.data()));
    task_data_seq->outputs_count.emplace_back(reference_max.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "max");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_max[0], global_max[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Max_2) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_max(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    const int count_size_vector = 120;
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_max.data()));
    task_data_par->outputs_count.emplace_back(global_max.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "max");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_max(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_max.data()));
    task_data_seq->outputs_count.emplace_back(reference_max.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "max");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_max[0], global_max[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Max_2_File) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_max(1, 0);
  // Create task_data
  auto task_data_par = std::make_shared<ppc::core::TaskData>();

  if (world.rank() == 0) {
    std::string line;
    std::ifstream test_file(ppc::core::GetAbsolutePath("all/example/data/test_mpi.txt"));
    if (test_file.is_open()) {
      getline(test_file, line);
    }
    test_file.close();

    const int count_size_vector = std::stoi(line);
    global_vec = GetRandomVector(count_size_vector);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_max.data()));
    task_data_par->outputs_count.emplace_back(global_max.size());
  }

  nesterov_a_test_task_mpi::TestMPITaskParallel test_mpi_task_parallel(task_data_par, "max");
  ASSERT_EQ(test_mpi_task_parallel.Validation(), true);
  test_mpi_task_parallel.PreProcessing();
  test_mpi_task_parallel.Run();
  test_mpi_task_parallel.PostProcessing();

  if (world.rank() == 0) {
    // Create data
    std::vector<int32_t> reference_max(1, 0);

    // Create task_data
    auto task_data_seq = std::make_shared<ppc::core::TaskData>();
    task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_seq->inputs_count.emplace_back(global_vec.size());
    task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(reference_max.data()));
    task_data_seq->outputs_count.emplace_back(reference_max.size());

    // Create Task
    nesterov_a_test_task_mpi::TestMPITaskSequential test_mpi_task_sequential(task_data_seq, "max");
    ASSERT_EQ(test_mpi_task_sequential.Validation(), true);
    test_mpi_task_sequential.PreProcessing();
    test_mpi_task_sequential.Run();
    test_mpi_task_sequential.PostProcessing();

    ASSERT_EQ(reference_max[0], global_max[0]);
  }
}
