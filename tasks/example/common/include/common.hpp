#pragma once

#include <utility>
#include <vector>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"

namespace nesterov_a_test_task {

using InType = std::vector<int>;
using OutType = std::vector<int>;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::core::Task<InType, OutType>;

}  // namespace nesterov_a_test_task
