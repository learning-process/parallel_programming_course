#include "core/util/include/util.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <array>
#include <filesystem>
#include <ranges>
#include <string>
#include <iostream>
#include <libenvpp/env.hpp>

//namespace {
//
//struct MpiEnvVar {
//  std::string_view name;
//  const char* description;
//};
//
//}  // namespace

std::string ppc::util::GetAbsolutePath(const std::string& relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

int ppc::util::GetNumThreads() {
  env::prefix pre("PPC");

  const auto num_threads_id = pre.register_variable<unsigned>("NUM_THREADS");

  auto validated = std::move(pre).parse_and_validate();

  if (!validated.ok()) {
    std::cerr << validated.warning_message() << "\n";
//    throw validated.error_message();
  }

  const auto num_threads = validated.get(num_threads_id);
  return num_threads ? static_cast<int>(*num_threads) : 1;
}

//const std::array<MpiEnvVar, 13> kMpiEnvVars = {{{.name = "OMPI_COMM_WORLD_SIZE", .description = "OpenMPI"},
//                                                {.name = "OMPI_UNIVERSE_SIZE", .description = "OpenMPI"},
//                                                {.name = "PMI_SIZE", .description = "MPICH/Intel MPI"},
//                                                {.name = "PMI_RANK", .description = "MPICH/Intel MPI"},
//                                                {.name = "PMI_FD", .description = "MPICH/Intel MPI"},
//                                                {.name = "HYDRA_CONTROL_FD", .description = "Hydra"},
//                                                {.name = "PMIX_RANK", .description = "Cray PMI/PMIx"},
//                                                {.name = "PMIX_NAMESPACE", .description = "Cray PMI/PMIx"},
//                                                {.name = "SLURM_PROCID", .description = "SLURM"},
//                                                {.name = "MPI_LOCALRANKID", .description = "IBM Spectrum MPI"},
//                                                {.name = "MSMPI_RANK", .description = "Microsoft MPI"},
//                                                {.name = "MSMPI_NODE_ID", .description = "Microsoft MPI"},
//                                                {.name = "MSMPI_LOCALRANK", .description = "Microsoft MPI"}}};

//bool ppc::util::IsUnderMpirun() {
//  auto pre = env::prefix("");
//  const auto parsed_and_validated_pre = pre.parse_and_validate();
//  return std::ranges::any_of(kMpiEnvVars, [&](const auto& env_var) {
//    const auto mpi_env_id = pre.register_variable<std::string>(env_var.name);
//
//    if (!parsed_and_validated_pre.ok()) {
//      std::cerr << parsed_and_validated_pre.warning_message();
//      throw parsed_and_validated_pre.error_message();
//    }
//
//    if (parsed_and_validated_pre.get(mpi_env_id)) {
//      return true;
//    } else {
//      return false;
//    }
//  });
//}
