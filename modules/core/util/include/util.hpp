#pragma once
#include <string>

namespace ppc::util {

std::string GetAbsolutePath(const std::string &relative_path);
int GetPPCNumThreads();

}  // namespace ppc::util
