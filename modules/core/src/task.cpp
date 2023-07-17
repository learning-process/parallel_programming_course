// Copyright 2023 Nesterov Alexander
#include "core/include/task.hpp"
#include <utility>

void ppc::core::Task::set_data(std::shared_ptr<TaskData> taskData_) {
    taskData = std::move(taskData_);
}

std::shared_ptr<ppc::core::TaskData> ppc::core::Task::get_data() const {
    return taskData;
}

ppc::core::Task::Task(std::shared_ptr<TaskData> taskData_) {
    set_data(std::move(taskData_));
}

void ppc::core::Task::internal_order_test(const std::string& str)  {
    std::cout << str << std::endl;
}

ppc::core::Task::~Task() {
}
