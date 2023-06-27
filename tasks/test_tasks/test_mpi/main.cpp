// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "./ops_mpi.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

TEST(Parallel_Operations_MPI, Test_Sum) {
    boost::mpi::communicator world;
    std::vector<int> global_vec;
    const int count_size_vector = 120;

    if (world.rank() == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_sum = getParallelOperations(global_vec, count_size_vector, "+");

    if (world.rank() == 0) {
        int reference_sum = getSequentialOperations(global_vec, "+");
        ASSERT_EQ(reference_sum, global_sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Diff) {
    boost::mpi::communicator world;
    std::vector<int> global_vec;
    const int count_size_vector = 120;

    if (world.rank() == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_diff = getParallelOperations(global_vec, count_size_vector, "-");

    if (world.rank() == 0) {
        int reference_diff = getSequentialOperations(global_vec, "-");
        ASSERT_EQ(reference_diff, global_diff);
    }
}

TEST(Parallel_Operations_MPI, Test_Diff_2) {
    boost::mpi::communicator world;
    std::vector<int> global_vec;
    const int count_size_vector = 120;

    if (world.rank() == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_diff = getParallelOperations(global_vec, count_size_vector, "-");

    if (world.rank() == 0) {
        int reference_diff = getSequentialOperations(global_vec, "-");
        ASSERT_EQ(reference_diff, global_diff);
    }
}

TEST(Parallel_Operations_MPI, Test_Max) {
    boost::mpi::communicator world;
    std::vector<int> global_vec;
    const int count_size_vector = 120;

    if (world.rank() == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_max;
    global_max = getParallelOperations(global_vec, count_size_vector, "max");

    if (world.rank() == 0) {
        int reference_max = getSequentialOperations(global_vec, "max");
        ASSERT_EQ(reference_max, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_Max_2) {
    boost::mpi::communicator world;
    std::vector<int> global_vec;
    const int count_size_vector = 120;

    if (world.rank() == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_max;
    global_max = getParallelOperations(global_vec, count_size_vector, "max");

    if (world.rank() == 0) {
        int reference_max = getSequentialOperations(global_vec, "max");
        ASSERT_EQ(reference_max, global_max);
    }
}

int main(int argc, char** argv) {
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    if (world.rank() != 0) {
        delete listeners.Release(listeners.default_result_printer());
    }
    return RUN_ALL_TESTS();
}
