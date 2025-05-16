#include "core/task/include/task.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std::chrono;

ppc::core::Task::Task(StateOfTesting state_of_testing) : state_of_testing_(state_of_testing) {
  auto custom_terminate = []() {
    std::cerr << "ORDER OF FUNCTIONS IS NOT RIGHT! \n"
                 "Expected - \"Validation\", \"PreProcessing\", \"Run\", \"PostProcessing\" \n";
    std::exit(404);
  };
  std::set_terminate(custom_terminate);
  functions_order_.clear();
}

bool ppc::core::Task::Validation() {
  InternalOrderTest(__builtin_FUNCTION());
  return ValidationImpl();
}

bool ppc::core::Task::PreProcessing() {
  InternalOrderTest(__builtin_FUNCTION());
  if (state_of_testing_ == StateOfTesting::kFunc) {
    InternalTimeTest(__builtin_FUNCTION());
  }
  return PreProcessingImpl();
}

bool ppc::core::Task::Run() {
  InternalOrderTest(__builtin_FUNCTION());
  return RunImpl();
}

bool ppc::core::Task::PostProcessing() {
  InternalOrderTest(__builtin_FUNCTION());
  if (state_of_testing_ == StateOfTesting::kFunc) {
    InternalTimeTest(__builtin_FUNCTION());
  }
  return PostProcessingImpl();
}

void ppc::core::Task::InternalOrderTest(const std::string &str) {
  functions_order_.push_back(str);
  if (str == "PostProcessing" && IsFullPipelineStage()) {
    functions_order_.clear();
  } else {
    was_worked_ = true;
  }
}

void ppc::core::Task::InternalTimeTest(const std::string &str) {
  if (str == "PreProcessing") {
    tmp_time_point_ = std::chrono::high_resolution_clock::now();
  }

  if (str == "PostProcessing") {
    auto duration = duration_cast<nanoseconds>(high_resolution_clock::now() - tmp_time_point_).count();
    auto diff = static_cast<double>(duration) * 1e-9;

    std::stringstream err_msg;
    if (diff < kMaxTestTime) {
      err_msg << "Test time:" << std::fixed << std::setprecision(10) << diff;
    } else {
      err_msg << "\nTask execute time need to be: ";
      err_msg << "time < " << kMaxTestTime << " secs.\n";
      err_msg << "Original time in secs: " << diff << '\n';
      throw std::runtime_error(err_msg.str().c_str());
    }
  }
}

bool ppc::core::Task::IsFullPipelineStage() {
  auto it = std::adjacent_find(functions_order_.begin() + 2,
                               functions_order_.begin() + static_cast<long>(functions_order_.size() - 2),
                               std::not_equal_to<>());

  return (functions_order_.size() >= 4 && functions_order_[0] == "Validation" &&
          functions_order_[1] == "PreProcessing" && functions_order_[2] == "Run" &&
          it == (functions_order_.begin() + static_cast<long>(functions_order_.size() - 2)) &&
          functions_order_[functions_order_.size() - 1] == "PostProcessing");
}

ppc::core::Task::~Task() {
  if (!functions_order_.empty() || !was_worked_) {
    std::terminate();
  } else {
    functions_order_.clear();
  }
}
