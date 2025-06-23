#include "core/util/include/util.hpp"

#include <gtest/gtest.h>

#include <libenvpp/detail/get.hpp>
#include <string_view>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(util_tests, extracts_correct_namespace) {
  constexpr std::string_view kNs = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(kNs, "my::nested");
}

TEST(util_tests, GetNumThreadsEnvUnsetReturn) {
  const auto num_threads_env_var = env::get<int>("PPC_NUM_THREADS");

  if (num_threads_env_var.has_value()) {
    EXPECT_EQ(ppc::util::GetNumThreads(), omp_get_num_threads());
  } else {
    GTEST_SKIP() << "PPC_NUM_THREADS environment variable is set, skipping test";
  }
}

namespace test_ns {
struct TypeInNamespace {};
}  // namespace test_ns

struct PlainType {};

TEST(GetNamespaceTest, ReturnsExpectedNamespace) {
  constexpr auto kNs = ppc::util::GetNamespace<test_ns::TypeInNamespace>();
  EXPECT_EQ(kNs, "test_ns");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PrimitiveType) {
  constexpr auto kNs = ppc::util::GetNamespace<int>();
  EXPECT_EQ(kNs, "");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PlainStruct) {
  constexpr auto kNs = ppc::util::GetNamespace<PlainType>();
  EXPECT_EQ(kNs, "");
}

namespace test_ns {
struct Nested {};
}  // namespace test_ns

TEST(GetNamespaceTest, ReturnsNamespaceCorrectly) {
  constexpr auto kNs = ppc::util::GetNamespace<test_ns::Nested>();
  EXPECT_EQ(kNs, "test_ns");
}

struct NoNamespaceType {};

TEST(GetNamespaceTest, NoNamespaceInType) {
  constexpr auto kNs = ppc::util::GetNamespace<NoNamespaceType>();
  EXPECT_EQ(kNs, "");
}

template <typename T>
struct NotATemplate {};

TEST(GetNamespaceTest, NoKeyInPrettyFunction) {
  constexpr auto kNs = ppc::util::GetNamespace<NotATemplate<void>>();
  EXPECT_EQ(kNs, "");
}

namespace crazy {
struct VeryLongTypeNameWithOnlyLettersAndUnderscores {};
}  // namespace crazy

TEST(GetNamespaceTest, NoTerminatorCharactersInPrettyFunction) {
  constexpr auto kNs = ppc::util::GetNamespace<crazy::VeryLongTypeNameWithOnlyLettersAndUnderscores>();
  EXPECT_EQ(kNs, "crazy");
}
