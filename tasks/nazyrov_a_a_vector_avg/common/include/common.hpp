#pragma once


#include <vector>

#include "task/include/task.hpp"

namespace nazyrov_a_a_vector_avg {

using InType = std::vector<int>;
using OutType = double;
using BaseTask = ppc::task::Task<InType, OutType>;


}  // namespace nazyrov_a_a_vector_avg