#pragma once

#include <chrono>
#include <thread>

#include "core/task/include/task.hpp"

namespace ppc::test::task {

template <typename InType, typename OutType>
class TestTask : public ppc::core::Task<InType, OutType> {
 public:
  explicit TestTask(const InType& in) { this->GetInput() = in; }

  bool ValidationImpl() override { return !this->GetInput().empty(); }

  bool PreProcessingImpl() override {
    this->GetOutput() = 0;
    return true;
  }

  bool RunImpl() override {
    for (unsigned i = 0; i < this->GetInput().size(); i++) {
      this->GetOutput() += this->GetInput()[i];
    }
    return true;
  }

  bool PostProcessingImpl() override { return true; }
};

template <typename InType, typename OutType>
class FakeSlowTask : public TestTask<InType, OutType> {
 public:
  explicit FakeSlowTask(const InType& in) : TestTask<InType, OutType>(in) {}

  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return TestTask<InType, OutType>::RunImpl();
  }
};

}  // namespace ppc::test::task
