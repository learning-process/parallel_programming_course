#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "task/include/task.hpp"

namespace example_common {

using InType = int;
using OutType = int;
struct TestCase {
  int value;
  std::string name;
};

using TestType = TestCase;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public ppc::task::Task<InType, OutType> {
 public:
  explicit BaseTask(InType input) : ppc::task::Task<InType, OutType>(std::move(input), kTaskType) {}

  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return kTaskType;
  }
};

}  // namespace example_common

namespace example_threads {

using example_common::InType;
using example_common::OutType;
using example_common::TestType;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public example_common::BaseTask<kTaskType> {
 public:
  using example_common::BaseTask<kTaskType>::BaseTask;

  static constexpr std::string_view GetTaskIdentifier() {
    return "example_threads";
  }
};

}  // namespace example_threads

namespace example_processes_t1 {

using example_common::InType;
using example_common::OutType;
using example_common::TestType;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public example_common::BaseTask<kTaskType> {
 public:
  using example_common::BaseTask<kTaskType>::BaseTask;

  static constexpr std::string_view GetTaskIdentifier() {
    return "example_processes_t1";
  }
};

}  // namespace example_processes_t1

namespace example_processes_t2 {

using example_common::InType;
using example_common::OutType;
using example_common::TestType;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public example_common::BaseTask<kTaskType> {
 public:
  using example_common::BaseTask<kTaskType>::BaseTask;

  static constexpr std::string_view GetTaskIdentifier() {
    return "example_processes_t2";
  }
};

}  // namespace example_processes_t2

namespace example_processes_t3 {

using example_common::InType;
using example_common::OutType;
using example_common::TestType;

template <ppc::task::TypeOfTask kTaskType>
class BaseTask : public example_common::BaseTask<kTaskType> {
 public:
  using example_common::BaseTask<kTaskType>::BaseTask;

  static constexpr std::string_view GetTaskIdentifier() {
    return "example_processes_t3";
  }
};

}  // namespace example_processes_t3
