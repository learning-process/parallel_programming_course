#include "util/include/util.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <libenvpp/detail/get.hpp>
#include <string>

namespace {

std::string GetAbsolutePath(const std::string &relative_path) {
  std::filesystem::path path = std::filesystem::path(PPC_PATH_TO_PROJECT) / "tasks" / relative_path;
  return path.string();
}

}  // namespace

std::string ppc::util::GetAbsoluteTaskPath(const std::string &id_path, const std::string &relative_path) {
  std::filesystem::path task_relative = std::filesystem::path(id_path) / "data" / relative_path;
  return GetAbsolutePath(task_relative.string());
}

int ppc::util::GetNumThreads() {
  const auto num_threads = env::get<int>("PPC_NUM_THREADS");
  if (num_threads.has_value()) {
    return num_threads.value();
  }
  return 1;
}

int ppc::util::GetNumProc() {
  const auto num_proc = env::get<int>("PPC_NUM_PROC");
  if (num_proc.has_value()) {
    return num_proc.value();
  }
  return 1;
}

double ppc::util::GetTaskMaxTime() {
  const auto val = env::get<double>("PPC_TASK_MAX_TIME");
  if (val.has_value()) {
    return val.value();
  }
  return 1.0;
}

double ppc::util::GetPerfMaxTime() {
  const auto val = env::get<double>("PPC_PERF_MAX_TIME");
  if (val.has_value()) {
    return val.value();
  }
  return 10.0;
}

// List of environment variables that signal the application is running under
// an MPI launcher. The array size must match the number of entries to avoid
// looking up empty environment variable names.
constexpr std::array<std::string_view, 10> kMpiEnvVars = {
    "OMPI_COMM_WORLD_SIZE", "OMPI_UNIVERSE_SIZE", "PMI_SIZE",     "PMI_RANK",   "PMI_FD",
    "HYDRA_CONTROL_FD",     "PMIX_RANK",          "SLURM_PROCID", "MSMPI_RANK", "MSMPI_LOCALRANK"};

bool ppc::util::IsUnderMpirun() {
  return std::ranges::any_of(kMpiEnvVars, [&](const auto &env_var) {
    const auto mpi_env = env::get<int>(env_var);
    return static_cast<bool>(mpi_env.has_value());
  });
}
