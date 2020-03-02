// Copyright 2018 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>

TEST(Sequential, Test_Sum) {
    const int count = 100;
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum++;
    }
    ASSERT_EQ(100, sum);
}
