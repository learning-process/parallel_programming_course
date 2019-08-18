// Copyright 2018 Nesterov Alexander
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./ops_mpi.h"

TEST(Parallel_Operations_MPI, Test_Sum) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    const int count_size_vector = 100;

    if (rank == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_sum = getParallelOperations(global_vec, count_size_vector, "+");

    if (rank == 0) {
        int reference_sum = getSequentialOperations(global_vec, "+");
        ASSERT_EQ(reference_sum, global_sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Diff) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    const int count_size_vector = 100;

    if (rank == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_diff = getParallelOperations(global_vec, count_size_vector, "-");

    if (rank == 0) {
        int reference_diff = getSequentialOperations(global_vec, "-");
        ASSERT_EQ(reference_diff, global_diff);
    }
}

TEST(Parallel_Operations_MPI, Test_Max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    const int count_size_vector = 100;

    if (rank == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_max;
    global_max = getParallelOperations(global_vec, count_size_vector, "max");

    if (rank == 0) {
        int reference_max = getSequentialOperations(global_vec, "max");
        ASSERT_EQ(reference_max, global_max);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
