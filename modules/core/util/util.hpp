#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ppc::util {

inline std::string GetAbsolutePath(const std::string &relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

inline int GetPPCNumThreads() {
#ifdef _WIN32
  size_t len;
  char omp_env[100];
  errno_t err = getenv_s(&len, omp_env, sizeof(omp_env), "OMP_NUM_THREADS");
  if (err != 0 || len == 0) {
    omp_env[0] = '\0';
  }
#else
  const char *omp_env = std::getenv("OMP_NUM_THREADS");
#endif
  int num_threads = (omp_env != nullptr) ? std::atoi(omp_env) : 1;
  return num_threads;
}

}  // namespace ppc::util
