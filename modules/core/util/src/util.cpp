#include "core/util/include/util.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <libenvpp/env.hpp>
#include <string>

std::string ppc::util::GetAbsolutePath(const std::string& relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

int ppc::util::GetNumThreads() {
  const auto num_threads = env::get<int>("PPC_NUM_THREADS");
  if (num_threads.has_value()) {
    return num_threads.value();
  }
  return 1;
}

constexpr std::array<std::string_view, 13> kMpiEnvVars = {
    "OMPI_COMM_WORLD_SIZE", "OMPI_UNIVERSE_SIZE", "PMI_SIZE",     "PMI_RANK",   "PMI_FD",
    "HYDRA_CONTROL_FD",     "PMIX_RANK",          "SLURM_PROCID", "MSMPI_RANK", "MSMPI_LOCALRANK"};

bool ppc::util::IsUnderMpirun() {
  return std::ranges::any_of(kMpiEnvVars, [&](const auto& env_var) {
    const auto mpi_env = env::get<int>(env_var);
    return static_cast<bool>(mpi_env.has_value());
  });
}
