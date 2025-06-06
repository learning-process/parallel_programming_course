#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <thread>

#include "core/util/include/util.hpp"

TEST(util_tests, check_unset_env) {
#ifndef _WIN32
  int save_var = ppc::util::GetNumThreads();

  unsetenv("PPC_NUM_THREADS");  // NOLINT(concurrency-mt-unsafe)

  EXPECT_EQ(ppc::util::GetNumThreads(), 1);

  setenv("PPC_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)
#else
  GTEST_SKIP();
#endif
}

TEST(util_tests, check_set_env) {
#ifndef _WIN32
  int save_var = ppc::util::GetNumThreads();

  const int num_threads = static_cast<int>(std::thread::hardware_concurrency());
  setenv("PPC_NUM_THREADS", std::to_string(num_threads).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)

  EXPECT_EQ(ppc::util::GetNumThreads(), num_threads);

  setenv("PPC_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)
#else
  GTEST_SKIP();
#endif
}
