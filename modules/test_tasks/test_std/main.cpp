// Copyright 2018 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "./ops_std.h"

TEST(Parallel_Operations_Std_Threads, Test_Sum) {
    std::vector<int> vec = getRandomVector(100);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_2) {
    std::vector<int> vec = getRandomVector(100);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Sum_3) {
    std::vector<int> vec = getRandomVector(100);
    int sequential_sum = getSequentialOperations(vec, "+");
    int parallel_sum = getParallelOperations(vec, "+");
    ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff) {
    std::vector<int> vec = getRandomVector(100);
    int sequential_diff = getSequentialOperations(vec, "-");
    int parallel_diff = getParallelOperations(vec, "-");
    ASSERT_EQ(sequential_diff, parallel_diff);
}

TEST(Parallel_Operations_Std_Threads, Test_Diff_2) {
    std::vector<int> vec = getRandomVector(100);
    int sequential_diff = getSequentialOperations(vec, "-");
    int parallel_diff = getParallelOperations(vec, "-");
    ASSERT_EQ(sequential_diff, parallel_diff);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
