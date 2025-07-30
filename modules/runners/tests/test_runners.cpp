#include <gtest/gtest.h>

#include <array>

#include "runners/include/runners.hpp"

TEST(RunnersTest, SimpleInit) {
  int argc = 1;
  std::array<char, 5> test_name = {"test"};
  std::array<char *, 2> argv = {test_name.data(), nullptr};
  EXPECT_EQ(ppc::runners::SimpleInit(argc, argv.data()), 0);
}
