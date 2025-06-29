#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>

#include "nlohmann/json_fwd.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4459)
#endif

#include <nlohmann/json.hpp>

/// @brief JSON namespace used for settings and config parsing.
using NlohmannJsonParseError = nlohmann::json::parse_error;
/// @brief JSON namespace used for settings and config typing.
using NlohmannJsonTypeError = nlohmann::json::type_error;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace ppc::util {

/**
 * @brief Returns the unqualified name of the current function.
 *
 * @param loc Source location, defaults to the current function.
 * @return Function name without namespaces or parameters.
 */
inline std::string FuncName(const std::source_location& loc = std::source_location::current()) {
  std::string s{loc.function_name()};
  if (auto p = s.find('('); p != std::string::npos) {
    s.resize(p);  // drop “(…)”
  }
  if (auto p = s.rfind("::"); p != std::string::npos) {
    s.erase(0, p + 2);  // drop namespaces
  }
  return s;
}

enum GTestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

std::string GetAbsoluteTaskPath(const std::string& id_path, const std::string& relative_path);
int GetNumThreads();

template <typename T>
constexpr std::string_view GetNamespace() {
#ifdef _MSC_VER
  constexpr std::string_view kFunc{__FUNCSIG__};
  constexpr std::string_view key = "GetNamespace<";
#else
  constexpr std::string_view kFunc{__PRETTY_FUNCTION__};
  constexpr std::string_view key = "T = ";
#endif

  auto start = kFunc.find(key);
  if (start == std::string_view::npos) return {};
  start += key.size();

  for (auto p : {"class ", "struct ", "enum ", "union "})
    if (kFunc.substr(start).starts_with(p)) start += std::string_view{p}.size();

  auto ns_type = kFunc.substr(start, kFunc.find(']', start) - start);
  auto pos = ns_type.rfind("::");
  return (pos != std::string_view::npos) ? ns_type.substr(0, pos) : std::string_view{};
}


inline std::shared_ptr<nlohmann::json> InitJSONPtr() { return std::make_shared<nlohmann::json>(); }

bool IsUnderMpirun();

}  // namespace ppc::util
