#include "core/task/include/task.hpp"

#include <chrono>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

ppc::core::Task::Task(StateOfTesting state_of_testing) : state_of_testing_(state_of_testing) {
  functions_order_.clear();
}

bool ppc::core::Task::Validation() {
  InternalOrderTest();
  return ValidationImpl();
}

bool ppc::core::Task::PreProcessing() {
  InternalOrderTest();
  return PreProcessingImpl();
}

bool ppc::core::Task::Run() {
  InternalOrderTest();
  return RunImpl();
}

bool ppc::core::Task::PostProcessing() {
  InternalOrderTest();
  return PostProcessingImpl();
}

void ppc::core::Task::InternalOrderTest(const std::string& str) {
  if (!functions_order_.empty() && str == functions_order_.back() && str == "Run") {
    return;
  }

  functions_order_.push_back(str);

  for (size_t i = 0; i < functions_order_.size(); i++) {
    if (functions_order_[i] != right_functions_order_[i % right_functions_order_.size()]) {
      functions_order_validation_ = false;
      throw std::invalid_argument("ORDER OF FUNCTIONS IS NOT RIGHT: \n" + std::string("Serial number: ") +
                                  std::to_string(i + 1) + "\n" + std::string("Your function: ") + functions_order_[i] +
                                  "\n" + std::string("Expected function: ") + right_functions_order_[i]);
    }
  }

  if (str == "PreProcessing" && state_of_testing_ == StateOfTesting::kFunc) {
    tmp_time_point_ = std::chrono::high_resolution_clock::now();
  }

  if (str == "PostProcessing" && state_of_testing_ == StateOfTesting::kFunc) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - tmp_time_point_).count();
    auto current_time = static_cast<double>(duration) * 1e-9;
    std::stringstream err_msg;
    if (current_time < kMaxTestTime) {
      err_msg << "Test time:" << std::fixed << std::setprecision(10) << current_time;
    } else {
      err_msg << "\nTask execute time need to be: ";
      err_msg << "time < " << kMaxTestTime << " secs.\n";
      err_msg << "Original time in secs: " << current_time << '\n';
      throw std::runtime_error(err_msg.str().c_str());
    }
  }
}

ppc::core::Task::~Task() {
  if (functions_order_.empty()) {
    std::cerr << "ORDER OF FUNCTIONS IS NOT RIGHT: No task functions were executed\n";
    std::terminate();
  }
  if (functions_order_validation_) {
    for (size_t i = 0; i < functions_order_.size(); i++) {
      if (functions_order_[i] != right_functions_order_[i % right_functions_order_.size()]) {
        std::cerr << "ORDER OF FUNCTIONS IS NOT RIGHT: \n"
                  << std::string("Serial number: ") << std::to_string(i + 1) << "\n"
                  << std::string("Your function: ") << functions_order_[i] << "\n"
                  << std::string("Expected function: ") << right_functions_order_[i] << "\n";
        std::terminate();
      }
    }
  }
  functions_order_.clear();
}
