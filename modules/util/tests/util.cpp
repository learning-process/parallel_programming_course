#include "util/include/util.hpp"

#include <gtest/gtest.h>

#include <libenvpp/detail/environment.hpp>
#include <libenvpp/detail/get.hpp>
#include <string>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(UtilTest, GetNamespace_WithNestedType_ReturnsCorrectNamespace) {
  std::string k_ns = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(k_ns, "my::nested");
}

TEST(UtilTest, GetNumThreads_WithOpenMPEnvironment_HandlesThreadControlCorrectly) {
  const auto num_threads_env_var = env::get<int>("PPC_NUM_THREADS");

  if (num_threads_env_var.has_value()) {
    // When PPC_NUM_THREADS is set, GetNumThreads() should return that value
    EXPECT_EQ(ppc::util::GetNumThreads(), num_threads_env_var.value());
  } else {
    // When PPC_NUM_THREADS is not set, GetNumThreads() should return 1
    // This is independent of OpenMP's thread count
    EXPECT_EQ(ppc::util::GetNumThreads(), 1);
  }
}

namespace test_ns {
struct TypeInNamespace {};
}  // namespace test_ns

struct PlainType {};

TEST(GetNamespaceTest, GetNamespace_WithNamespacedType_ReturnsExpectedNamespace) {
  std::string k_ns = ppc::util::GetNamespace<test_ns::TypeInNamespace>();
  EXPECT_EQ(k_ns, "test_ns");
}

TEST(GetNamespaceTest, GetNamespace_WithPrimitiveType_ReturnsEmptyString) {
  std::string k_ns = ppc::util::GetNamespace<int>();
  EXPECT_EQ(k_ns, "");
}

TEST(GetNamespaceTest, GetNamespace_WithPlainStruct_ReturnsEmptyString) {
  std::string k_ns = ppc::util::GetNamespace<PlainType>();
  EXPECT_EQ(k_ns, "");
}

namespace test_ns {
struct Nested {};
}  // namespace test_ns

TEST(GetNamespaceTest, GetNamespace_WithNestedStruct_ReturnsNamespaceCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<test_ns::Nested>();
  EXPECT_EQ(k_ns, "test_ns");
}

struct NoNamespaceType {};

TEST(GetNamespaceTest, GetNamespace_WithNoNamespaceType_ReturnsEmptyString) {
  std::string k_ns = ppc::util::GetNamespace<NoNamespaceType>();
  EXPECT_EQ(k_ns, "");
}

template <typename T>
struct NotATemplate {};

TEST(GetNamespaceTest, GetNamespace_WithTemplateType_ReturnsEmptyString) {
  std::string k_ns = ppc::util::GetNamespace<NotATemplate<void>>();
  EXPECT_EQ(k_ns, "");
}

namespace crazy {
struct VeryLongTypeNameWithOnlyLettersAndUnderscores {};
}  // namespace crazy

TEST(GetNamespaceTest, GetNamespace_WithLongTypeName_ReturnsCorrectNamespace) {
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
