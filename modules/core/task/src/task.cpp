// Copyright 2023 Nesterov Alexander
#include "core/task/include/task.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

void ppc::core::Task::set_data(std::shared_ptr<TaskData> taskData_) {
  taskData_->state_of_testing = TaskData::StateOfTesting::FUNC;
  functions_order.clear();
  taskData = std::move(taskData_);
}

std::shared_ptr<ppc::core::TaskData> ppc::core::Task::get_data() const { return taskData; }

ppc::core::Task::Task(std::shared_ptr<TaskData> taskData_) { set_data(std::move(taskData_)); }

void ppc::core::Task::internal_order_test(const std::string& str) {
  if (!functions_order.empty() && str == functions_order.back() && str == "run") return;

  functions_order.push_back(str);

  for (size_t i = 0; i < functions_order.size(); i++) {
    if (functions_order[i] != right_functions_order[i % right_functions_order.size()]) {
      throw std::invalid_argument("ORDER OF FUCTIONS IS NOT RIGHT: \n" + std::string("Serial number: ") +
                                  std::to_string(i + 1) + "\n" + std::string("Yours function: ") + functions_order[i] +
                                  "\n" + std::string("Expected function: ") + right_functions_order[i]);
    }
  }

  if (str == "pre_processing" && taskData->state_of_testing == TaskData::StateOfTesting::FUNC) {
    tmp_time_point = std::chrono::high_resolution_clock::now();
  }

  if (str == "post_processing" && taskData->state_of_testing == TaskData::StateOfTesting::FUNC) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - tmp_time_point).count();
    auto current_time = static_cast<double>(duration) * 1e-9;
    if (current_time > max_test_time) {
      std::cerr << "Current test work more than " << max_test_time << " secs: " << current_time << std::endl;
      EXPECT_TRUE(current_time < max_test_time);
    }
  }
}

ppc::core::Task::~Task() { functions_order.clear(); }
