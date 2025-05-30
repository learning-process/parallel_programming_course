#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <thread>

#include "core/util/include/util.hpp"

#ifdef _WIN32
#include <Windows.h>
inline int unsetenv(const char* name) { return SetEnvironmentVariableA(name, NULL) ? 0 : -1; }
inline int setenv(const char* name, const char* value, int /*overwrite*/) {
  return SetEnvironmentVariableA(name, value) ? 0 : -1;
}
#endif

TEST(util_tests, check_unset_env) {
  int save_var = ppc::util::GetPPCNumThreads();

  unsetenv("PPC_NUM_THREADS");  // NOLINT(concurrency-mt-unsafe)

  EXPECT_EQ(ppc::util::GetPPCNumThreads(), 1);

  setenv("PPC_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)
}

TEST(util_tests, check_set_env) {
  int save_var = ppc::util::GetPPCNumThreads();

  const int num_threads = static_cast<int>(std::thread::hardware_concurrency());
  setenv("PPC_NUM_THREADS", std::to_string(num_threads).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)

  EXPECT_EQ(ppc::util::GetPPCNumThreads(), num_threads);

  setenv("PPC_NUM_THREADS", std::to_string(save_var).c_str(), 1);  // NOLINT(concurrency-mt-unsafe)
}
