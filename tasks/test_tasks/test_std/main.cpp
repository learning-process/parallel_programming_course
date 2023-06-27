// Copyright 2018 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "../../../3rdparty/unapproved/unapproved.h"
#include "./ops_std.h"

TEST(Parallel_Operations_Std_Threads, Test_Sum) {
    const int nthreads = std::thread::hardware_concurrency() * 10;
    std::vector<int> vec = getRandomVector(nthreads);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_2) {
    const int nthreads = std::thread::hardware_concurrency() * 11;
    std::vector<int> vec = getRandomVector(nthreads);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_3) {
    const int nthreads = std::thread::hardware_concurrency() * 13;
    std::vector<int> vec = getRandomVector(nthreads);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff) {
    const int nthreads = std::thread::hardware_concurrency() * 14;
    std::vector<int> vec = getRandomVector(nthreads);
    int sequential_diff = getSequentialOperations(vec, "-");
    int parallel_diff = getParallelOperations(vec, "-");
    ASSERT_EQ(sequential_diff, parallel_diff);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff_2) {
    const int nthreads = std::thread::hardware_concurrency() * 15;
    std::vector<int> vec = getRandomVector(nthreads);
    int sequential_diff = getSequentialOperations(vec, "-");
    int parallel_diff = getParallelOperations(vec, "-");
    ASSERT_EQ(sequential_diff, parallel_diff);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
