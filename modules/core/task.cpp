// Copyright 2023 Nesterov Alexander
#include "../../../modules/core/task.hpp"
#include <cstdlib>
#include <utility>

double ppc::core::Task::perf() {
    return static_cast<double>(std::rand() % 10);
}

void ppc::core::Task::set_data(std::shared_ptr<TaskData> taskData_) {
    taskData = std::move(taskData_);
}

std::shared_ptr<ppc::core::TaskData> ppc::core::Task::get_data() const {
    return taskData;
}

ppc::core::Task::Task(std::shared_ptr<TaskData> taskData_) : taskData(std::move(taskData_)) {}
