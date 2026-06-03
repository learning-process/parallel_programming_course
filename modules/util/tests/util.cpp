#include "util/include/util.hpp"

#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <libenvpp/detail/environment.hpp>
#include <libenvpp/detail/get.hpp>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "omp.h"
#include "task/include/task.hpp"
#include "util/include/func_test_util.hpp"

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

namespace {

using FuncTestUtilParam = ppc::util::FuncTestParam<int, int, int>;

FuncTestUtilParam MakeFuncTestUtilParam(const std::string &test_name, int value) {
  return FuncTestUtilParam{[](int) -> ppc::task::TaskPtr<int, int> { return {}; }, test_name, value};
}

void ExpectSingleNonFatalFailureContains(const ::testing::TestPartResultArray &failures, std::string_view message) {
  ASSERT_EQ(failures.size(), 1);
  const ::testing::TestPartResult &failure = failures.GetTestPartResult(0);
  EXPECT_EQ(failure.type(), ::testing::TestPartResult::kNonFatalFailure);
  EXPECT_NE(std::string_view(failure.message()).find(message), std::string_view::npos);
}

}  // namespace

TEST(FuncTestUtil, RunTestCasesWithTagAcceptsBareTags) {
  const auto test_tasks = std::make_tuple(MakeFuncTestUtilParam("example_threads_seq_enabled", 1),
                                          MakeFuncTestUtilParam("example_threads_tbb_enabled", 2),
                                          MakeFuncTestUtilParam("example_threads_tbb_disabled", 3));

  std::vector<int> visited_params;
  ppc::util::RunTestCasesWithTag(test_tasks, "tbb", [&](const auto &test_param) {
    visited_params.push_back(std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(test_param));
  });

  const std::vector<int> expected_params{2, 3};
  EXPECT_EQ(visited_params, expected_params);
}

TEST(FuncTestUtil, RunTestCasesWithTagFailsWhenTagIsMissing) {
  const auto test_tasks = std::make_tuple(MakeFuncTestUtilParam("example_threads_seq_enabled", 1));

  bool callback_was_called = false;
  ::testing::TestPartResultArray failures;
  {
    ::testing::ScopedFakeTestPartResultReporter reporter(
        ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD, &failures);
    ppc::util::RunTestCasesWithTag(test_tasks, "omp", [&](const auto & /*test_param*/) { callback_was_called = true; });
  }

  ExpectSingleNonFatalFailureContains(failures, "No functional test cases matched tag: omp");
  EXPECT_FALSE(callback_was_called);
}
