#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace nesterov_a_test_task_processes_3 {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace nesterov_a_test_task_processes_3
