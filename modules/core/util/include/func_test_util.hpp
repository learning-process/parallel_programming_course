#pragma once

#include <gtest/gtest.h>
#include <mpi.h>
#include <omp.h>
#include <tbb/tick_count.h>

#include <csignal>
#include <filesystem>
#include <fstream>

#include "core/perf/include/perf.hpp"
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
    ASSERT_FALSE(std::get<GTestParamIndex::kNameTest>(test_param).find("unknown") != std::string::npos);
    if (std::get<GTestParamIndex::kNameTest>(test_param).find("disabled") != std::string::npos) {
      GTEST_SKIP();
    }

    task_ = std::get<GTestParamIndex::kTaskGetter>(test_param)(GetTestInputData());
    ASSERT_TRUE(task_->Validation());
    ASSERT_TRUE(task_->PreProcessing());
    ASSERT_TRUE(task_->Run());
    ASSERT_TRUE(task_->PostProcessing());
    std::cout << "task_->GetInput() = " << task_->GetInput() << std::endl;
    std::cout << "task_->GetOutput() = " << task_->GetOutput() << std::endl;
    ASSERT_TRUE(CheckTestOutputData(task_->GetOutput()));
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

#define INIT_TASK_GENERATOR(InTypeParam, SizesParam)                                                         \
  template <typename Task, std::size_t... Is>                                                                \
  auto GenTaskTuplesImpl(std::index_sequence<Is...>) {                                                       \
    return std::make_tuple(                                                                                  \
        std::make_tuple(ppc::core::TaskGetter<Task, InTypeParam>,                                            \
                        std::string(ppc::util::GetNamespace<Task>()) + std::string("_") +                    \
                            ppc::core::GetStringTaskType(Task::GetStaticTypeOfTask(), PPC_STUDENT_SETTINGS), \
                        SizesParam[Is])...);                                                                 \
  }                                                                                                          \
                                                                                                             \
  template <typename Task>                                                                                   \
  auto TaskListGenerator() {                                                                                 \
    return GenTaskTuplesImpl<Task>(std::make_index_sequence<SizesParam.size()>{});                           \
  }

}  // namespace ppc::util
