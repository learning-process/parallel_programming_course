#include <gtest/gtest.h>

#include <array>

#include "runners/include/runners.hpp"

TEST(RunnersTest, SimpleInit) {
  int argc = 1;
  std::array<char, 5> test_name{{'t', 'e', 's', 't', '\0'}};
  std::array<char *, 2> argv = {test_name.data(), nullptr};
  EXPECT_EQ(ppc::runners::SimpleInit(argc, argv.data()), 0);
}
