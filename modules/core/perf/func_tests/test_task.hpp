#pragma once

#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include "core/task/include/task.hpp"

namespace ppc::test::perf {

template <class T>
class TestTask : public ppc::core::Task {
 public:
  explicit TestTask(const std::vector<T>& in) : input_(in) {}

  bool ValidationImpl() override {
    return !input_.empty();
  }

  bool PreProcessingImpl() override {
    output_ = 0;
    return true;
  }

  bool RunImpl() override {
    for (unsigned i = 0; i < input_.size(); i++) {
      output_ += input_[i];
    }
    return true;
  }

  bool PostProcessingImpl() override {
    return true;
  }

  T Get() { return output_; }

 private:
  std::vector<T> input_{};
  T output_;
};

template <class T>
class FakePerfTask : public TestTask<T> {
 public:
  explicit FakePerfTask(const std::vector<T>& in) : TestTask<T>(in) {}

  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(11));
    return TestTask<T>::RunImpl();
  }
};

}  // namespace ppc::test::perf
