#include "core/util/include/util.hpp"

#include <gtest/gtest.h>

#include <libenvpp/detail/get.hpp>
#include <string_view>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(util_tests, extracts_correct_namespace) {
  std::string kNs = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(kNs, "my::nested");
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
  std::string kNs = ppc::util::GetNamespace<test_ns::TypeInNamespace>();
  EXPECT_EQ(kNs, "test_ns");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PrimitiveType) {
  std::string kNs = ppc::util::GetNamespace<int>();
  EXPECT_EQ(kNs, "");
}

TEST(GetNamespaceTest, ReturnsEmptyIfNoNamespace_PlainStruct) {
  std::string kNs = ppc::util::GetNamespace<PlainType>();
  EXPECT_EQ(kNs, "");
}

namespace test_ns {
struct Nested {};
}  // namespace test_ns

TEST(GetNamespaceTest, ReturnsNamespaceCorrectly) {
  std::string kNs = ppc::util::GetNamespace<test_ns::Nested>();
  EXPECT_EQ(kNs, "test_ns");
}

struct NoNamespaceType {};

TEST(GetNamespaceTest, NoNamespaceInType) {
  std::string kNs = ppc::util::GetNamespace<NoNamespaceType>();
  EXPECT_EQ(kNs, "");
}

template <typename T>
struct NotATemplate {};

TEST(GetNamespaceTest, NoKeyInPrettyFunction) {
  std::string kNs = ppc::util::GetNamespace<NotATemplate<void>>();
  EXPECT_EQ(kNs, "");
}

namespace crazy {
struct VeryLongTypeNameWithOnlyLettersAndUnderscores {};
}  // namespace crazy

TEST(GetNamespaceTest, NoTerminatorCharactersInPrettyFunction) {
  std::string kNs = ppc::util::GetNamespace<crazy::VeryLongTypeNameWithOnlyLettersAndUnderscores>();
  EXPECT_EQ(kNs, "crazy");
}
