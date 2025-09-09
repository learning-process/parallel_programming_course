#include "util/include/util.hpp"

#include <gtest/gtest.h>

#include <libenvpp/detail/environment.hpp>
#include <libenvpp/detail/get.hpp>
#include <string>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(UtilTests, ExtractsCorrectNamespace) {
  std::string k_ns = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(k_ns, "my::nested");
}

TEST(UtilTests, ThreadsControlCheckOpenmpDisabledValgrind) {
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

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespacePrimitiveType) {
  std::string k_ns = ppc::util::GetNamespace<int>();
  EXPECT_EQ(k_ns, "");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespacePlainStruct) {
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
  const auto old = env::get<double>("PPC_TASK_MAX_TIME");
  if (old.has_value()) {
    env::detail::delete_environment_variable("PPC_TASK_MAX_TIME");
  }
  EXPECT_DOUBLE_EQ(ppc::util::GetTaskMaxTime(), 1.0);
  if (old.has_value()) {
    env::detail::set_environment_variable("PPC_TASK_MAX_TIME", std::to_string(*old));
  }
}

TEST(GetTaskMaxTime, ReadsFromEnvironment) {
  env::detail::set_scoped_environment_variable scoped("PPC_TASK_MAX_TIME", "2.5");
  EXPECT_DOUBLE_EQ(ppc::util::GetTaskMaxTime(), 2.5);
}

TEST(GetPerfMaxTime, ReturnsDefaultWhenUnset) {
  const auto old = env::get<double>("PPC_PERF_MAX_TIME");
  if (old.has_value()) {
    env::detail::delete_environment_variable("PPC_PERF_MAX_TIME");
  }
  EXPECT_DOUBLE_EQ(ppc::util::GetPerfMaxTime(), 10.0);
  if (old.has_value()) {
    env::detail::set_environment_variable("PPC_PERF_MAX_TIME", std::to_string(*old));
  }
}

TEST(GetPerfMaxTime, ReadsFromEnvironment) {
  env::detail::set_scoped_environment_variable scoped("PPC_PERF_MAX_TIME", "12.5");
  EXPECT_DOUBLE_EQ(ppc::util::GetPerfMaxTime(), 12.5);
}

TEST(GetNumProc, ReturnsDefaultWhenUnset) {
  const auto old = env::get<int>("PPC_NUM_PROC");
  if (old.has_value()) {
    env::detail::delete_environment_variable("PPC_NUM_PROC");
  }
  EXPECT_EQ(ppc::util::GetNumProc(), 1);
  if (old.has_value()) {
    env::detail::set_environment_variable("PPC_NUM_PROC", std::to_string(*old));
  }
}

TEST(GetNumProc, ReadsFromEnvironment) {
  env::detail::set_scoped_environment_variable scoped("PPC_NUM_PROC", "4");
  EXPECT_EQ(ppc::util::GetNumProc(), 4);
}
