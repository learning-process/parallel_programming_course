#include "core/task/include/task.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>
#include <iomanip>

void ppc::core::Task::set_data(TaskDataPtr taskData_) {
  taskData_->state_of_testing = TaskData::StateOfTesting::FUNC;
  functions_order.clear();
  taskData = std::move(taskData_);
}

ppc::core::TaskDataPtr ppc::core::Task::get_data() const { return taskData; }

ppc::core::Task::Task(TaskDataPtr taskData_) { set_data(std::move(taskData_)); }

bool ppc::core::Task::validation() {
  internal_order_test();
  return validation_impl();
}

bool ppc::core::Task::pre_processing() {
  internal_order_test();
  return pre_processing_impl();
}

bool ppc::core::Task::run() {
  internal_order_test();
  return run_impl();
}

bool ppc::core::Task::post_processing() {
  internal_order_test();
  return post_processing_impl();
}

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
    if (current_time < max_test_time) {
      std::cout << "Test time:" << std::fixed << std::setprecision(10) << current_time;
    } else {
      std::stringstream err_msg;
      err_msg << "\nTask execute time need to be: ";
      err_msg << "time < " << max_test_time << " secs.\n";
      err_msg << "Original time in secs: " << current_time << std::endl;
      throw std::runtime_error(err_msg.str().c_str());
    }
  }
}

ppc::core::Task::~Task() { functions_order.clear(); }
