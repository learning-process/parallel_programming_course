#include "core/util/include/util.hpp"

#include <cstdlib>
#ifdef _WIN32
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#endif

#include <filesystem>
#include <string>

std::string ppc::util::GetAbsolutePath(const std::string &relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

int ppc::util::GetPPCNumThreads() {
#ifdef _WIN32
  size_t len;
  char omp_env[100];
  errno_t err = getenv_s(&len, omp_env, sizeof(omp_env), "PPC_NUM_THREADS");
  if (err != 0 || len == 0) {
    omp_env[0] = '\0';
  }
  int num_threads = std::atoi(omp_env);
#else
  const char *omp_env = std::getenv("PPC_NUM_THREADS");  // NOLINT(concurrency-mt-unsafe)
  int num_threads = (omp_env != nullptr) ? std::atoi(omp_env) : 1;
#endif
  return num_threads;
}
