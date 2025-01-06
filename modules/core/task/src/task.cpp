#include "core/task/include/task.hpp"

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <format>

constexpr auto right_functions_order = std::array{"validate", "pre_process", "run", "post_process"};

bool ppc::core::BaseTask::validate() {
  internal_order_test();
  return validation_impl();
}

bool ppc::core::BaseTask::pre_process() {
  internal_order_test();
  return pre_processing_impl();
}

bool ppc::core::BaseTask::run() {
  internal_order_test();
  return run_impl();
}

bool ppc::core::BaseTask::post_process() {
  internal_order_test();
  return post_processing_impl();
}

void ppc::core::BaseTask::internal_order_test(const std::string& str) {
  if (!functions_order.empty() && str == "run" && str == functions_order.back()) return;

  functions_order.push_back(str);

  for (size_t i = 0; i < functions_order.size(); i++) {
    if (functions_order[i] != right_functions_order[i % right_functions_order.size()]) {
      throw std::invalid_argument(
          std::format("ORDER OF FUNCTIONS IS NOT RIGHT: \n"
                      "Serial number: {}\n"
                      "Yours function: {}\n"
                      "Expected function: {}",
                      i + 1, functions_order[i], right_functions_order[i]));
    }
  }

  if (str == "pre_process" && testing_mode == TestingMode::Func) {
    tmp_time_point = std::chrono::high_resolution_clock::now();
  }

  if (str == "post_process" && testing_mode == TestingMode::Func) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - tmp_time_point).count();
    auto current_time = static_cast<double>(duration) * 1e-9;
    if (current_time > max_test_time) {
      std::cerr << "Current test work more than " << max_test_time << " secs: " << current_time << std::endl;
      EXPECT_TRUE(current_time < max_test_time);
    }
  }
}
