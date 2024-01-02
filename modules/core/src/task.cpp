// Copyright 2023 Nesterov Alexander
#include "core/include/task.hpp"

#include <stdexcept>
#include <utility>

void ppc::core::Task::set_data(std::shared_ptr<TaskData> taskData_) {
  functions_order.clear();
  taskData = std::move(taskData_);
}

std::shared_ptr<ppc::core::TaskData> ppc::core::Task::get_data() const { return taskData; }

ppc::core::Task::Task(std::shared_ptr<TaskData> taskData_) { set_data(std::move(taskData_)); }

void ppc::core::Task::internal_order_test(const std::string& str) {
  if (!functions_order.empty() && str == functions_order.back() && str == "run") return;

  functions_order.push_back(str);

  for (auto i = 0; i < functions_order.size(); i++) {
    if (functions_order[i] != right_functions_order[i % right_functions_order.size()]) {
      throw std::invalid_argument("ORDER OF FUCTIONS IS NOT RIGHT: \n" + std::string("Serial number: ") +
                                  std::to_string(i + 1) + "\n" + std::string("Yours function: ") + functions_order[i] +
                                  "\n" + std::string("Expected function: ") + right_functions_order[i]);
    }
  }
}

ppc::core::Task::~Task() { functions_order.clear(); }
