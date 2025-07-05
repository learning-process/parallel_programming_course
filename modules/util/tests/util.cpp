#include "util/include/util.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <libenvpp/detail/get.hpp>
#include <string>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(util_tests, extracts_correct_namespace) {
  std::string k_ns = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(k_ns, "my::nested");
}

TEST(util_tests, threads_control_check_openmp_disabled_valgrind) {
  const auto num_threads_env_var = env::get<int>("PPC_NUM_THREADS");

  EXPECT_EQ(ppc::util::GetNumThreads(), omp_get_max_threads());
}

namespace test_ns {
struct TypeInNamespace {};
}  // namespace test_ns

struct PlainType {};

TEST(GetNamespaceTest, ReturnsExpectedNamespace) {
  std::string k_ns = ppc::util::GetNamespace<test_ns::TypeInNamespace>();
  EXPECT_EQ(k_ns, "test_ns");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PrimitiveType) {
  std::string k_ns = ppc::util::GetNamespace<int>();
  EXPECT_EQ(k_ns, "");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PlainStruct) {
  std::string k_ns = ppc::util::GetNamespace<PlainType>();
  EXPECT_EQ(k_ns, "");
}

namespace test_ns {
struct Nested {};
}  // namespace test_ns

TEST(GetNamespaceTest, ReturnsNamespaceCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<test_ns::Nested>();
  EXPECT_EQ(k_ns, "test_ns");
}

struct NoNamespaceType {};

TEST(GetNamespaceTest, NoNamespaceInType) {
  std::string k_ns = ppc::util::GetNamespace<NoNamespaceType>();
  EXPECT_EQ(k_ns, "");
}

template <typename T>
struct NotATemplate {};

TEST(GetNamespaceTest, NoKeyInPrettyFunction) {
  std::string k_ns = ppc::util::GetNamespace<NotATemplate<void>>();
  EXPECT_EQ(k_ns, "");
}

namespace crazy {
struct VeryLongTypeNameWithOnlyLettersAndUnderscores {};
}  // namespace crazy

TEST(GetNamespaceTest, NoTerminatorCharactersInPrettyFunction) {
  std::string k_ns = ppc::util::GetNamespace<crazy::VeryLongTypeNameWithOnlyLettersAndUnderscores>();
  EXPECT_EQ(k_ns, "crazy");
}

TEST(GetTaskMaxTime, ReturnsDefaultWhenUnset) {
  const char* old = std::getenv("PPC_TASK_MAX_TIME");
  unsetenv("PPC_TASK_MAX_TIME");
  EXPECT_DOUBLE_EQ(ppc::util::GetTaskMaxTime(), 1.0);
  if (old) {
    setenv("PPC_TASK_MAX_TIME", old, 1);
  }
}

TEST(GetTaskMaxTime, ReadsFromEnvironment) {
  const char* old = std::getenv("PPC_TASK_MAX_TIME");
  setenv("PPC_TASK_MAX_TIME", "2.5", 1);
  EXPECT_DOUBLE_EQ(ppc::util::GetTaskMaxTime(), 2.5);
  if (old) {
    setenv("PPC_TASK_MAX_TIME", old, 1);
  } else {
    unsetenv("PPC_TASK_MAX_TIME");
  }
}

TEST(GetPerfMaxTime, ReturnsDefaultWhenUnset) {
  const char* old = std::getenv("PPC_PERF_MAX_TIME");
  unsetenv("PPC_PERF_MAX_TIME");
  EXPECT_DOUBLE_EQ(ppc::util::GetPerfMaxTime(), 10.0);
  if (old) {
    setenv("PPC_PERF_MAX_TIME", old, 1);
  }
}

TEST(GetPerfMaxTime, ReadsFromEnvironment) {
  const char* old = std::getenv("PPC_PERF_MAX_TIME");
  setenv("PPC_PERF_MAX_TIME", "12.5", 1);
  EXPECT_DOUBLE_EQ(ppc::util::GetPerfMaxTime(), 12.5);
  if (old) {
    setenv("PPC_PERF_MAX_TIME", old, 1);
  } else {
    unsetenv("PPC_PERF_MAX_TIME");
  }
}
