#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <thread>

#include "core/util/include/util.hpp"

TEST(util_tests, check_unset_env) {
#ifndef _WIN32
  int save_var = ppc::util::GetPPCNumThreads();

  unsetenv("OMP_NUM_THREADS");  // NOLINT(misc-include-cleaner)

  EXPECT_EQ(ppc::util::GetPPCNumThreads(), 1);

  setenv("OMP_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(misc-include-cleaner)
#else
  GTEST_SKIP();
#endif
}

TEST(util_tests, check_set_env) {
#ifndef _WIN32
  int save_var = ppc::util::GetPPCNumThreads();

  const int num_threads = static_cast<int>(std::thread::hardware_concurrency());
  setenv("OMP_NUM_THREADS", std::to_string(num_threads).c_str(), 1);  // NOLINT(misc-include-cleaner)

  EXPECT_EQ(ppc::util::GetPPCNumThreads(), num_threads);

  setenv("OMP_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(misc-include-cleaner)
#else
  GTEST_SKIP();
#endif
}
