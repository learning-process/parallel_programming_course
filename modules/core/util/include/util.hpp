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
#if defined(_MSC_VER)
  constexpr auto func = std::string_view{__FUNCSIG__};
  auto start = func.find("GetNamespace<") + 13;
  for (auto p : {"class ", "struct ", "enum ", "union "})
    if (func.substr(start).starts_with(p)) start += p.size();
#else
  constexpr auto func = std::string_view{__PRETTY_FUNCTION__};
  auto start = func.find("T = ") + 4;
#endif
  auto end = func.find_first_of(";]> ,>", start);
  if (end == std::string_view::npos) return {};
  auto ns_end = func.rfind("::", end);
  return (ns_end != std::string_view::npos && ns_end > start) ? func.substr(start, ns_end - start) : std::string_view{};
}

inline std::shared_ptr<nlohmann::json> InitJSONPtr() { return std::make_shared<nlohmann::json>(); }

bool IsUnderMpirun();

}  // namespace ppc::util
