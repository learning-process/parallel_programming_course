#include "core/task/include/task.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

void ppc::core::Task::SetData(TaskDataPtr task_data_ptr) {
  task_data_ptr->state_of_testing = TaskData::StateOfTesting::kFunc;
  functions_order_.clear();
  this->task_data = std::move(task_data_ptr);
}

ppc::core::TaskDataPtr ppc::core::Task::GetData() const { return task_data; }

ppc::core::Task::Task(TaskDataPtr task_data) { SetData(std::move(task_data)); }

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
      throw std::invalid_argument("ORDER OF FUCTIONS IS NOT RIGHT: \n" + std::string("Serial number: ") +
                                  std::to_string(i + 1) + "\n" + std::string("Yours function: ") + functions_order_[i] +
                                  "\n" + std::string("Expected function: ") + right_functions_order_[i]);
    }
  }

  if (str == "PreProcessing" && task_data->state_of_testing == TaskData::StateOfTesting::kFunc) {
    tmp_time_point_ = std::chrono::high_resolution_clock::now();
  }

  if (str == "PostProcessing" && task_data->state_of_testing == TaskData::StateOfTesting::kFunc) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - tmp_time_point_).count();
    auto current_time = static_cast<double>(duration) * 1e-9;
    if (current_time < max_test_time_) {
      std::cout << "Test time:" << std::fixed << std::setprecision(10) << current_time;
    } else {
      std::stringstream err_msg;
      err_msg << "\nTask execute time need to be: ";
      err_msg << "time < " << max_test_time_ << " secs.\n";
      err_msg << "Original time in secs: " << current_time << '\n';
      throw std::runtime_error(err_msg.str().c_str());
    }
  }
}

ppc::core::Task::~Task() { functions_order_.clear(); }
