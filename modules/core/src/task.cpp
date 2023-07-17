// Copyright 2023 Nesterov Alexander
#include "core/include/task.hpp"
#include <utility>
#include <stdexcept>

void ppc::core::Task::set_data(std::shared_ptr<TaskData> taskData_) {
    taskData = std::move(taskData_);

    mtx->lock();
    functions_order.clear();
    mtx->unlock();

    internal_order_test();
}

std::shared_ptr<ppc::core::TaskData> ppc::core::Task::get_data() const {
    return taskData;
}

ppc::core::Task::Task(std::shared_ptr<TaskData> taskData_) {
    set_data(std::move(taskData_));
}

void ppc::core::Task::internal_order_test(const std::string& str)  {
    mtx->lock();
    functions_order.push_back(str);
    mtx->unlock();
}

ppc::core::Task::~Task() {
    if (functions_order.size() != 5) {
        throw std::length_error("ORDER OF FUCTIONS IS NOT RIGHT: " + std::to_string(functions_order.size()));
    }
    else {
        for (int i = 0; i < functions_order.size(); i++) {
            if (functions_order[i] != right_functions_order[i]) {
                throw std::invalid_argument("ORDER OF FUCTIONS IS NOT RIGHT: \n" +
                     std::string("Serial number: ") + std::to_string(i + 1) + "\n" +
                     std::string("Yours function: ") + functions_order[i] + "\n" +
                     std::string("Expected function: ") + right_functions_order[i]);
            }
        }
    }
}
