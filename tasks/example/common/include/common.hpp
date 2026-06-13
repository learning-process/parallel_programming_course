#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace example_common {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public ppc::task::Task<InType, OutType> {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return kTaskType;
  }
};

}  // namespace example_common

namespace example_threads {

using example_common::BaseTask;
using example_common::InType;
using example_common::OutType;
using example_common::TestType;

}  // namespace example_threads

namespace example_processes_t1 {

using example_common::BaseTask;
using example_common::InType;
using example_common::OutType;
using example_common::TestType;

}  // namespace example_processes_t1

namespace example_processes_t2 {

using example_common::BaseTask;
using example_common::InType;
using example_common::OutType;
using example_common::TestType;

}  // namespace example_processes_t2

namespace example_processes_t3 {

using example_common::BaseTask;
using example_common::InType;
using example_common::OutType;
using example_common::TestType;

}  // namespace example_processes_t3
