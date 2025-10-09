#pragma once

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <typeinfo>
#ifdef __GNUG__
#  include <cxxabi.h>
#endif

#include "nlohmann/json_fwd.hpp"

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4459)
#endif

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

enum class GTestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

std::string GetAbsoluteTaskPath(const std::string &id_path, const std::string &relative_path);
int GetNumThreads();
int GetNumProc();
double GetTaskMaxTime();
double GetPerfMaxTime();

template <typename T>
std::string GetNamespace() {
  std::string name = typeid(T).name();
#ifdef __GNUC__
  int status = 0;
  std::unique_ptr<char, void (*)(void *)> demangled{abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status),
                                                    std::free};
  name = (status == 0) ? demangled.get() : name;
#endif
#ifdef _MSC_VER
  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto &prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      break;
    }
  }
  name.erase(0, name.find_first_not_of(' '));
#endif
  auto pos = name.rfind("::");
  return (pos != std::string::npos) ? name.substr(0, pos) : std::string{};
}

inline std::shared_ptr<nlohmann::json> InitJSONPtr() {
  return std::make_shared<nlohmann::json>();
}

bool IsUnderMpirun();

}  // namespace ppc::util
