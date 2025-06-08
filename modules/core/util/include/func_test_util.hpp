#pragma once

#include <gtest/gtest.h>
#include <omp.h>
#include <tbb/tick_count.h>

#include <concepts>
#include <csignal>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"

namespace ppc::util {

template <typename InType, typename OutType, typename TestType = void>
using FuncTestParam = std::tuple<std::function<ppc::core::TaskPtr<InType, OutType>(InType)>, std::string, TestType>;

template <typename InType, typename OutType, typename TestType = void>
using GTestFuncParam = ::testing::TestParamInfo<FuncTestParam<InType, OutType, TestType>>;

template <typename T, typename TestType>
concept HasPrintTestParam = requires(TestType value) {
  { T::PrintTestParam(value) } -> std::same_as<std::string>;
};

template <typename InType, typename OutType, typename TestType = void>
class BaseRunFuncTests : public ::testing::TestWithParam<FuncTestParam<InType, OutType, TestType>> {
 public:
  virtual bool CheckTestOutputData(OutType& output_data) = 0;
  virtual InType GetTestInputData() = 0;

  template <typename Derived>
  static void RequireStaticInterface() {
    static_assert(HasPrintTestParam<Derived, TestType>,
                  "Derived class must implement: static std::string PrintTestParam(TestType)");
  }

  template <typename Derived>
  static std::string PrintFuncTestName(const GTestFuncParam<InType, OutType, TestType>& info) {
    RequireStaticInterface<Derived>();
    TestType test_param = std::get<ppc::util::GTestParamIndex::kTestParams>(info.param);
    return std::get<GTestParamIndex::kNameTest>(info.param) + "_" + Derived::PrintTestParam(test_param);
  }

 protected:
  void ExecuteTest(FuncTestParam<InType, OutType, TestType> test_param) {
    const std::string& test_name = std::get<GTestParamIndex::kNameTest>(test_param);

    ValidateTestName(test_name);

    if (IsTestDisabled(test_name)) {
      GTEST_SKIP();
    }

    if (ShouldSkipNonMpiTask(test_name)) {
      std::cerr << "kALL and kMPI tasks are not under mpirun\n";
      GTEST_SKIP();
    }

    InitializeAndRunTask(test_param);
  }

  void ValidateTestName(const std::string& test_name) { EXPECT_FALSE(test_name.find("unknown") != std::string::npos); }

  bool IsTestDisabled(const std::string& test_name) { return test_name.find("disabled") != std::string::npos; }

  bool ShouldSkipNonMpiTask(const std::string& test_name) {
    auto contains_substring = [&](const std::string& substring) {
      return test_name.find(substring) != std::string::npos;
    };

    return !ppc::util::IsUnderMpirun() && (contains_substring("_all") || contains_substring("_mpi"));
  }

  void InitializeAndRunTask(const FuncTestParam<InType, OutType, TestType>& test_param) {
    task_ = std::get<GTestParamIndex::kTaskGetter>(test_param)(GetTestInputData());

    EXPECT_TRUE(task_->Validation());
    EXPECT_TRUE(task_->PreProcessing());
    EXPECT_TRUE(task_->Run());
    EXPECT_TRUE(task_->PostProcessing());
    EXPECT_TRUE(CheckTestOutputData(task_->GetOutput()));
  }

 private:
  ppc::core::TaskPtr<InType, OutType> task_;
};

template <typename Tuple, std::size_t... Is>
auto ExpandToValuesImpl(const Tuple& t, std::index_sequence<Is...> /*unused*/) {
  return ::testing::Values(std::get<Is>(t)...);
}

template <typename Tuple>
auto ExpandToValues(const Tuple& t) {
  constexpr std::size_t kN = std::tuple_size_v<Tuple>;
  return ExpandToValuesImpl(t, std::make_index_sequence<kN>{});
}

template <typename Task, typename InType, typename SizesContainer, std::size_t... Is>
auto GenTaskTuplesImpl(const SizesContainer& sizes, const std::string& settings_path,
                       std::index_sequence<Is...> /*unused*/) {
  return std::make_tuple(std::make_tuple(ppc::core::TaskGetter<Task, InType>,
                                         std::string(GetNamespace<Task>()) + "_" +
                                             ppc::core::GetStringTaskType(Task::GetStaticTypeOfTask(), settings_path),
                                         sizes[Is])...);
}

template <typename Task, typename InType, typename SizesContainer>
auto TaskListGenerator(const SizesContainer& sizes, const std::string& settings_path) {
  return GenTaskTuplesImpl<Task, InType>(sizes, settings_path,
                                         std::make_index_sequence<std::tuple_size_v<std::decay_t<SizesContainer>>>{});
}

template <typename Task, typename InType, typename SizesContainer>
constexpr auto AddFuncTask(const SizesContainer& sizes, const std::string& settings_path) {
  return TaskListGenerator<Task, InType>(sizes, settings_path);
}

}  // namespace ppc::util
