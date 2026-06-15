#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "task/include/task.hpp"
#include "util/include/util.hpp"

namespace ppc::util {

inline ppc::task::TaskDescriptor MakeTaskDescriptor(std::string_view task_namespace, ppc::task::TypeOfTask task_type,
                                                    const std::string &settings_path,
                                                    std::string_view settings_task_path = {}) {
  const auto status = ppc::task::GetTaskStatus(task_type, settings_path, settings_task_path);
  const std::string_view task_type_name = ppc::task::TypeOfTaskToString(task_type);
  const auto task_name = task_type == ppc::task::TypeOfTask::kUnknown
                             ? std::string(task_type_name)
                             : std::string(task_type_name) + "_" + std::string(ppc::task::StatusOfTaskToString(status));
  return {.type = task_type,
          .status = status,
          .category = ppc::task::TaskCategoryFromSettingsPath(settings_task_path),
          .display_name = std::string(task_namespace) + "_" + task_name};
}

template <typename TestParam>
const ppc::task::TaskDescriptor &GetTaskDescriptor(const TestParam &test_param) {
  return std::get<static_cast<std::size_t>(GTestParamIndex::kTaskDescriptor)>(test_param);
}

inline bool IsMpiTaskType(ppc::task::TypeOfTask type) {
  return type == ppc::task::TypeOfTask::kMPI || type == ppc::task::TypeOfTask::kALL;
}

inline bool IsOshTaskType(ppc::task::TypeOfTask type) {
  return type == ppc::task::TypeOfTask::kOSH;
}

}  // namespace ppc::util
