#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

#include "nlohmann/json_fwd.hpp"

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4459)
#endif

#include <gtest/gtest.h>

#include <libenvpp/detail/environment.hpp>
#include <libenvpp/detail/get.hpp>
#include <nlohmann/json.hpp>

/// @brief JSON namespace used for settings and config parsing.
using NlohmannJsonParseError = nlohmann::json::parse_error;
/// @brief JSON namespace used for settings and config typing.
using NlohmannJsonTypeError = nlohmann::json::type_error;
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

namespace ppc::util {

/// @brief Utility class for tracking destructor failure across tests.
/// @details Provides thread-safe methods to set, unset, and check the failure flag.
class DestructorFailureFlag {
 public:
  /// @brief Marks that a destructor failure has occurred.
  static void Set() {
    failure_flag.store(true);
  }

  /// @brief Clears the destructor failure flag.
  static void Unset() {
    failure_flag.store(false);
  }

  /// @brief Checks if a destructor failure was recorded.
  /// @return True if failure occurred, false otherwise.
  static bool Get() {
    return failure_flag.load();
  }

 private:
  inline static std::atomic<bool> failure_flag{false};
};

class PerformanceFailureFlag {
 public:
  static void Set() {
    failure_flag.store(true);
  }

  static void Unset() {
    failure_flag.store(false);
  }

  static bool Get() {
    return failure_flag.load();
  }

 private:
  inline static std::atomic<bool> failure_flag{false};
};

std::string GetAbsoluteTaskPath(const std::string &id_path, const std::string &relative_path);
int GetNumThreads();
int GetNumProc();
double GetTaskMaxTime();
double GetPerfMaxTime();
double GetTimeMPI();
int GetMPIRank();
void ConfigureMpiEnvironment();
void SynchronizeMpiRanks();

inline std::shared_ptr<nlohmann::json> InitJSONPtr() {
  return std::make_shared<nlohmann::json>();
}

bool IsUnderMpirun();

namespace test {

[[nodiscard]] inline std::string SanitizeToken(std::string_view token_sv) {
  std::string token{token_sv};
  auto is_allowed = [](char c) -> bool {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.';
  };
  std::ranges::replace(token, ' ', '_');
  for (char &ch : token) {
    if (!is_allowed(ch)) {
      ch = '_';
    }
  }
  return token;
}

class ScopedPerTestEnv {
 public:
  explicit ScopedPerTestEnv(const std::string &token)
      : set_uid_("PPC_TEST_UID", token), set_tmp_("PPC_TEST_TMPDIR", CreateTmpDir(token)) {}

 private:
  static std::string CreateTmpDir(const std::string &token) {
    namespace fs = std::filesystem;
    auto make_rank_suffix = []() -> std::string {
      // Derive rank from common MPI env vars without including MPI headers
      constexpr std::array<std::string_view, 5> kRankVars = {"OMPI_COMM_WORLD_RANK", "PMI_RANK", "PMIX_RANK",
                                                             "SLURM_PROCID", "MSMPI_RANK"};
      for (auto name : kRankVars) {
        if (auto r = env::get<int>(name); r.has_value() && r.value() >= 0) {
          return std::string("_rank_") + std::to_string(r.value());
        }
      }
      return std::string{};
    };
    const std::string rank_suffix = IsUnderMpirun() ? make_rank_suffix() : std::string{};
    const fs::path tmp = fs::temp_directory_path() / (std::string("ppc_test_") + token + rank_suffix);
    std::error_code ec;
    fs::create_directories(tmp, ec);
    (void)ec;
    return tmp.string();
  }

  env::detail::set_scoped_environment_variable set_uid_;
  env::detail::set_scoped_environment_variable set_tmp_;
};

[[nodiscard]] inline std::string MakeCurrentGTestToken(std::string_view fallback_name) {
  const auto *unit = ::testing::UnitTest::GetInstance();
  const auto *info = (unit != nullptr) ? unit->current_test_info() : nullptr;
  std::ostringstream os;
  if (info != nullptr) {
    os << info->test_suite_name() << "." << info->name();
  } else {
    os << fallback_name;
  }
  return SanitizeToken(os.str());
}

inline ScopedPerTestEnv MakePerTestEnvForCurrentGTest(std::string_view fallback_name) {
  return ScopedPerTestEnv(MakeCurrentGTestToken(fallback_name));
}

}  // namespace test

}  // namespace ppc::util
