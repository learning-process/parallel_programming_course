#pragma once

#include <gtest/gtest.h>
#include <omp.h>
#include <tbb/tick_count.h>

#include <concepts>
#include <csignal>
#include <functional>
#include <string>
#include <tuple>

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

  void ExecuteTest(FuncTestParam<InType, OutType, TestType> test_param) {
    const std::string& test_name = std::get<GTestParamIndex::kNameTest>(test_param);

    validateTestName(test_name);

    if (isTestDisabled(test_name)) {
      GTEST_SKIP();
    }

    if (shouldSkipNonMpiTask(test_name)) {
      std::cerr << "kALL and kMPI tasks are not under mpirun\n";
      GTEST_SKIP();
    }

    initializeAndRunTask(test_param);
  }

 private:
  static constexpr std::string UNKNOWN_TEST = "unknown";
  static constexpr std::string DISABLED_TEST = "disabled";
  static constexpr std::string ALL_TASK = "_all";
  static constexpr std::string MPI_TASK = "_mpi";

  void validateTestName(const std::string& test_name) {
    EXPECT_FALSE(test_name.find(UNKNOWN_TEST) != std::string::npos);
  }

  bool isTestDisabled(const std::string& test_name) { return test_name.find(DISABLED_TEST) != std::string::npos; }

  bool shouldSkipNonMpiTask(const std::string& test_name) {
    auto containsSubstring = [&](const std::string& substring) {
      return test_name.find(substring) != std::string::npos;
    };

    return !ppc::util::IsUnderMpirun() && (containsSubstring(ALL_TASK) || containsSubstring(MPI_TASK));
  }

  void initializeAndRunTask(const FuncTestParam<InType, OutType, TestType>& test_param) {
    task_ = std::get<GTestParamIndex::kTaskGetter>(test_param)(GetTestInputData());

    EXPECT_TRUE(task_->Validation());
    EXPECT_TRUE(task_->PreProcessing());
    EXPECT_TRUE(task_->Run());
    EXPECT_TRUE(task_->PostProcessing());
    EXPECT_TRUE(CheckTestOutputData(task_->GetOutput()));
  }

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

#define INIT_FUNC_TASK_GENERATOR(InTypeParam, SizesParam, SettingsPath)                              \
  template <typename Task, std::size_t... Is>                                                        \
  auto GenTaskTuplesImpl(std::index_sequence<Is...>) {                                               \
    return std::make_tuple(                                                                          \
        std::make_tuple(ppc::core::TaskGetter<Task, InTypeParam>,                                    \
                        std::string(ppc::util::GetNamespace<Task>()) + std::string("_") +            \
                            ppc::core::GetStringTaskType(Task::GetStaticTypeOfTask(), SettingsPath), \
                        SizesParam[Is])...);                                                         \
  }                                                                                                  \
                                                                                                     \
  template <typename Task>                                                                           \
  auto TaskListGenerator() {                                                                         \
    return GenTaskTuplesImpl<Task>(std::make_index_sequence<SizesParam.size()>{});                   \
  }

#define ADD_FUNC_TASK(TASK) TaskListGenerator<TASK>()  // std::tuple<>()

}  // namespace ppc::util
