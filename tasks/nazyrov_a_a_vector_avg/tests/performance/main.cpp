#include <gtest/gtest.h>
#include <mpi.h>

#include <numeric>
#include <random>
#include <vector>

#include "../../common/include/common.hpp"
#include "../../mpi/include/ops_mpi.hpp"
#include "../../seq/include/ops_seq.hpp"

namespace nazyrov_a_a_vector_avg {

TEST(NazyrovVectorAvgPerfTest, SeqPerformance) {
  const int size = 1000000;
  InType input(size);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1, 100);
  for (int i = 0; i < size; ++i) {
    input[i] = dist(gen);
  }

  auto task = std::make_shared<VectorAvgSEQ>(input);

  auto start = std::chrono::high_resolution_clock::now();
  ASSERT_TRUE(task->Validation());
  ASSERT_TRUE(task->PreProcessing());
  ASSERT_TRUE(task->Run());
  ASSERT_TRUE(task->PostProcessing());
  auto end = std::chrono::high_resolution_clock::now();

  double elapsed = std::chrono::duration<double>(end - start).count();
  std::cout << "SEQ time for " << size << " elements: " << elapsed << "s\n";
  EXPECT_GT(elapsed, 0);
}

TEST(NazyrovVectorAvgPerfTest, MpiPerformance) {
  const int size = 1000000;
  InType input(size);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1, 100);
  for (int i = 0; i < size; ++i) {
    input[i] = dist(gen);
  }

  auto task = std::make_shared<VectorAvgMPI>(input);

  auto start = std::chrono::high_resolution_clock::now();
  ASSERT_TRUE(task->Validation());
  ASSERT_TRUE(task->PreProcessing());
  ASSERT_TRUE(task->Run());
  ASSERT_TRUE(task->PostProcessing());
  auto end = std::chrono::high_resolution_clock::now();

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "MPI time for " << size << " elements: " << elapsed << "s\n";
    EXPECT_GT(elapsed, 0);
  }
}

}  // namespace nazyrov_a_a_vector_avg
