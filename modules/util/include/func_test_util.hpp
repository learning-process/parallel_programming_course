#pragma once

#include <gtest/gtest.h>
#include <tbb/tick_count.h>

#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "task/include/task.hpp"
#include "util/include/util.hpp"

namespace ppc::util {

template <typename InType, typename OutType, typename TestType = void>
using FuncTestParam = std::tuple<std::function<ppc::task::TaskPtr<InType, OutType>(InType)>, std::string, TestType>;

template <typename InType, typename OutType, typename TestType = void>
using GTestFuncParam = ::testing::TestParamInfo<FuncTestParam<InType, OutType, TestType>>;

template <typename T, typename TestType>
concept HasPrintTestParam = requires(TestType value) {
  { T::PrintTestParam(value) } -> std::same_as<std::string>;
};

template <typename TestTasksList, typename RunTestCase>
void RunTestCasesWithTag(const TestTasksList &test_tasks_list, std::string_view task_tag, RunTestCase run_test_case);

template <typename InType, typename OutType, typename TestType = void>
/// @brief Base class for running functional tests on parallel tasks.
/// @tparam InType Type of input data.
/// @tparam OutType Type of output data.
/// @tparam TestType Type of the test case or parameter.
class BaseRunFuncTests : public ::testing::TestWithParam<FuncTestParam<InType, OutType, TestType>> {
 public:
  template <typename Derived>
  static void RequireStaticInterface() {
    static_assert(HasPrintTestParam<Derived, TestType>,
                  "Derived class must implement: static std::string PrintTestParam(TestType)");
  }

  template <typename Derived>
  static std::string PrintFuncTestName(const GTestFuncParam<InType, OutType, TestType> &info) {
    RequireStaticInterface<Derived>();
    TestType test_param = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(info.param);
    return std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(info.param) + "_" +
           Derived::PrintTestParam(test_param);
  }

 protected:
  virtual bool CheckTestOutputData(OutType &output_data) = 0;
  /// @brief Provides input data for the task.
  /// @return Initialized input data.
  virtual InType GetTestInputData() = 0;

  virtual void RunTestCase(const FuncTestParam<InType, OutType, TestType> &test_param) {
    ExecuteTest(test_param);
  }

  template <typename TestTasksList>
  void RunTestCasesWithTag(const TestTasksList &test_tasks_list, std::string_view task_tag) {
    ppc::util::RunTestCasesWithTag(test_tasks_list, task_tag,
                                   [this](const auto &test_param) { RunTestCase(test_param); });
  }

  void ExecuteTest(FuncTestParam<InType, OutType, TestType> test_param) {
    const std::string &test_name = std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(test_param);

    ValidateTestName(test_name);

    const auto test_env_scope = ppc::util::test::MakePerTestEnvForCurrentGTest(test_name);

    if (IsTestDisabled(test_name)) {
      GTEST_SKIP();
    }

    if (ShouldSkipNonMpiTask(test_name)) {
      std::cerr << "kALL and kMPI tasks are not under mpirun\n";
      GTEST_SKIP();
    }

    InitializeAndRunTask(test_param);
  }

  void ValidateTestName(const std::string &test_name) {
    EXPECT_FALSE(test_name.contains("unknown"));
  }

  bool IsTestDisabled(const std::string &test_name) {
    return test_name.contains("disabled");
  }

  bool ShouldSkipNonMpiTask(const std::string &test_name) {
    auto contains_substring = [&](const std::string &substring) { return test_name.contains(substring); };

    return !ppc::util::IsUnderMpirun() && (contains_substring("_all") || contains_substring("_mpi"));
  }

  /// @brief Initializes task instance and runs it through the full pipeline.
  void InitializeAndRunTask(const FuncTestParam<InType, OutType, TestType> &test_param) {
    task_ = std::get<static_cast<std::size_t>(GTestParamIndex::kTaskGetter)>(test_param)(GetTestInputData());
    ExecuteTaskPipeline();
  }

  void ExecuteTaskPipeline() {
    ValidateTask();
    RunTask();
    CheckTaskOutput();
  }

  void ValidateTask() {
    EXPECT_TRUE(task_->Validation());
    SynchronizeMpiRanks();
    EXPECT_TRUE(task_->PreProcessing());
  }

  void RunTask() {
    EXPECT_TRUE(task_->Run());
    EXPECT_TRUE(task_->PostProcessing());
  }

  void CheckTaskOutput() {
    EXPECT_TRUE(CheckTestOutputData(task_->GetOutput()));
  }

 private:
  ppc::task::TaskPtr<InType, OutType> task_;
};

namespace detail {

[[nodiscard]] inline std::string MakeFuncTestTaskTagPattern(std::string_view task_tag) {
  std::string tag_pattern{task_tag};
  if (!tag_pattern.starts_with('_')) {
    tag_pattern.insert(0, 1, '_');
  }
  if (!tag_pattern.ends_with('_')) {
    tag_pattern.push_back('_');
  }
  return tag_pattern;
}

}  // namespace detail

template <typename TestTasksList, typename RunTestCase>
void RunTestCasesWithTag(const TestTasksList &test_tasks_list, std::string_view task_tag, RunTestCase run_test_case) {
  if (task_tag.empty()) {
    ADD_FAILURE() << "Functional test task tag must not be empty";
    return;
  }

  const std::string task_tag_pattern = detail::MakeFuncTestTaskTagPattern(task_tag);
  bool has_matching_task = false;
  std::apply([&](const auto &...test_params) {
    auto run_if_tagged = [&](const auto &test_param) {
      const std::string &test_name = std::get<static_cast<std::size_t>(GTestParamIndex::kNameTest)>(test_param);
      if (test_name.contains(task_tag_pattern)) {
        has_matching_task = true;
        std::invoke(run_test_case, test_param);
      }
    };
    (run_if_tagged(test_params), ...);
  }, test_tasks_list);
  EXPECT_TRUE(has_matching_task) << "No functional test cases matched tag: " << std::string(task_tag);
}

template <typename Tuple, std::size_t... Is>
auto ExpandToValuesImpl(const Tuple &t, std::index_sequence<Is...> /*unused*/) {
  return ::testing::Values(std::get<Is>(t)...);
}

template <typename Tuple>
auto ExpandToValues(const Tuple &t) {
  constexpr std::size_t kN = std::tuple_size_v<Tuple>;
  return ExpandToValuesImpl(t, std::make_index_sequence<kN>{});
}

template <typename Task, typename InType, typename SizesContainer, std::size_t... Is>
auto GenTaskTuplesImpl(const SizesContainer &sizes, const std::string &settings_path,
                       std::string_view settings_task_path, std::index_sequence<Is...> /*unused*/) {
  return std::make_tuple(
      std::make_tuple(ppc::task::TaskGetter<Task, InType>,
                      std::string(GetNamespace<Task>()) + "_" +
                          ppc::task::GetStringTaskType(Task::GetStaticTypeOfTask(), settings_path, settings_task_path),
                      std::get<Is>(sizes))...);
}

template <typename Task, typename InType, typename SizesContainer>
auto TaskListGenerator(const SizesContainer &sizes, const std::string &settings_path,
                       std::string_view settings_task_path = {}) {
  return GenTaskTuplesImpl<Task, InType>(sizes, settings_path, settings_task_path,
                                         std::make_index_sequence<std::tuple_size_v<std::decay_t<SizesContainer>>>{});
}

template <typename Task, typename InType, typename SizesContainer>
constexpr auto AddFuncTask(const SizesContainer &sizes, const std::string &settings_path,
                           std::string_view settings_task_path = {}) {
  return TaskListGenerator<Task, InType>(sizes, settings_path, settings_task_path);
}

}  // namespace ppc::util
