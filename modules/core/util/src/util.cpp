#define _CRT_SECURE_NO_WARNINGS
#include "core/util/include/util.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

std::string ppc::util::GetAbsolutePath(const std::string &relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

int ppc::util::GetPPCNumThreads() {
  const char *env = std::getenv("PPC_NUM_THREADS");  // NOLINT(concurrency-mt-unsafe)
  if ((env != nullptr) && (*env != 0)) {
    char *endptr = nullptr;
    long val = std::strtol(env, &endptr, 10);
    if (endptr != env && val > 0) {
      return static_cast<int>(val);
    }
  }
  return 1;
}
