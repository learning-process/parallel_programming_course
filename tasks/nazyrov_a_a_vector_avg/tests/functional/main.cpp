#include <gtest/gtest.h>
#include <mpi.h>

#include <numeric>
#include <random>
#include <vector>

#include "../../common/include/common.hpp"
#include "../../mpi/include/ops_mpi.hpp"
#include "../../seq/include/ops_seq.hpp"

namespace nazyrov_a_a_vector_avg {

class NazyrovVectorAvgTest : public ::testing::TestWithParam<TestType> {
 public:
  static void SetUpTestSuite() {
    // Инициализация MPI один раз для всех тестов
    int initialized = 0;
    MPI_Initialized(&initialized);
    if (!initialized) {
      int argc = 0;
      char **argv = nullptr;
      MPI_Init(&argc, &argv);
    }
  }

  static void TearDownTestSuite() {
    int finalized = 0;
    MPI_Finalized(&finalized);
    if (!finalized) {
      MPI_Finalize();
    }
  }

 protected:
  void SetUp() override {
    int size = std::get<0>(GetParam());
    input_.resize(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    for (int i = 0; i < size; ++i) {
      input_[i] = dist(gen);
    }

    expected_ = std::accumulate(input_.begin(), input_.end(), 0.0) / size;
  }

  InType input_;
  OutType expected_;
};

TEST_P(NazyrovVectorAvgTest, SeqTest) {
  auto task = std::make_shared<VectorAvgSEQ>(input_);
  ASSERT_TRUE(task->Validation());
  ASSERT_TRUE(task->PreProcessing());
  ASSERT_TRUE(task->Run());
  ASSERT_TRUE(task->PostProcessing());
  EXPECT_NEAR(task->GetOutput(), expected_, 1e-9);
}

TEST_P(NazyrovVectorAvgTest, MpiTest) {
  auto task = std::make_shared<VectorAvgMPI>(input_);
  ASSERT_TRUE(task->Validation());
  ASSERT_TRUE(task->PreProcessing());
  ASSERT_TRUE(task->Run());
  ASSERT_TRUE(task->PostProcessing());

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    EXPECT_NEAR(task->GetOutput(), expected_, 1e-9);
  }
}

INSTANTIATE_TEST_SUITE_P(VectorAvgTests, NazyrovVectorAvgTest,
                         ::testing::Values(std::make_tuple(10, "small"), std::make_tuple(100, "medium"),
                                           std::make_tuple(1000, "large")));

}  // namespace nazyrov_a_a_vector_avg
