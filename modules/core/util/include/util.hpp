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
 * @brief Obtain the simple function name (e.g. "PreProcessing" or "foo")
 *        from the full signature returned by std::source_location.
 *
 * @param loc  Source location info (file, line, full signature).
 *             Defaults to the call site via std::source_location::current().
 * @return A std::string with only the function’s unqualified name.
 *
 * @details
 * - On GCC/Clang, function_name() returns a pretty signature
 *   including namespaces, templates, and parameters.
 * - On MSVC, it also includes return type and calling convention.
 * - This routine removes any leading scope qualifiers (“::…”) and
 *   drops everything from the first '(' onward.
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
#if defined(__clang__) || defined(__GNUC__)
  constexpr std::string_view kFunc = __PRETTY_FUNCTION__;
  constexpr std::string_view kKey = "T = ";

  auto start = kFunc.find(kKey);
  if (start == std::string_view::npos) {
    return {};
  }
  start += kKey.size();

  auto end = kFunc.find_first_of(";]> ,", start);
  if (end == std::string_view::npos) {
    return {};
  }

  auto full_type = kFunc.substr(start, end - start);

  auto ns_end = full_type.rfind("::");
  if (ns_end == std::string_view::npos) {
    return {};
  }

  return full_type.substr(0, ns_end);

#elif defined(_MSC_VER)
  constexpr std::string_view kFunc = __FUNCSIG__;
  constexpr std::string_view kKey = "GetNamespace<";

  auto start = kFunc.find(kKey);
  if (start == std::string_view::npos) return {};
  start += kKey.size();

  constexpr std::string_view prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (auto prefix : prefixes) {
    if (kFunc.substr(start, prefix.size()) == prefix) {
      start += prefix.size();
      break;
    }
  }

  auto end = kFunc.find('>', start);
  if (end == std::string_view::npos) return {};

  auto full_type = kFunc.substr(start, end - start);

  auto ns_end = full_type.rfind("::");
  if (ns_end == std::string_view::npos) return {};

  return full_type.substr(0, ns_end);

#else
  static_assert([] { return false; }(), "Unsupported compiler");
  return {};
#endif
}

inline std::shared_ptr<nlohmann::json> InitJSONPtr() { return std::make_shared<nlohmann::json>(); }

bool IsUnderMpirun();

}  // namespace ppc::util
