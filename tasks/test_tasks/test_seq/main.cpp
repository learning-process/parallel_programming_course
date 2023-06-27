// Copyright 2018 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>

TEST(Sequential, Test_Sum_10) {
    const int count = 10;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(count, sum);
}

TEST(Sequential, Test_Sum_20) {
    const int count = 20;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(count, sum);
}

TEST(Sequential, Test_Sum_50) {
    const int count = 20;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(count, sum);
}

TEST(Sequential, Test_Sum_70) {
    const int count = 20;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(count, sum);
}

TEST(Sequential, Test_Sum_100) {
    const int count = 100;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(count, sum);
}
