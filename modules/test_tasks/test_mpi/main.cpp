// Copyright 2018 Nesterov Alexander
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "./ops_mpi.h"

TEST(BasicMPI, PassOnAllRanks) {
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank;
    MPI_Comm_rank(comm, &rank);
    EXPECT_EQ(rank, getMpiRank(comm));
}

TEST(BasicMPI, FailOnAllRanks) {
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank;
    MPI_Comm_rank(comm, &rank);
    EXPECT_EQ(rank, getMpiRankPlusOne(comm));
}

TEST(BasicMPI, FailExceptOnRankZero) {
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank;
    MPI_Comm_rank(comm, &rank);
    EXPECT_EQ(0, getZero(comm));
}

// TEST(BasicMPI, PassExceptOnRankZero) {
//     MPI_Comm comm = MPI_COMM_WORLD;
//     int rank;
//     MPI_Comm_rank(comm, &rank);
//     EXPECT_EQ(rank, getNonzeroMpiRank(comm));
// }

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
