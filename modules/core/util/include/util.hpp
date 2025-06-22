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
 * @brief Extract the bare function name (e.g. "PreProcessing" or "foo")
 *        from a full signature provided by std::source_location,
 *        working both on GCC/Clang and MSVC.
 *
 * @param loc  Source location (file, line, pretty-signature).
 *             Defaults to the call site via std::source_location::current().
 * @return A std::string containing only the function name.
 *
 * @note  On GCC/Clang, loc.function_name() yields a “pretty” signature
 *        including namespaces, templates and parameter list.
 *        On MSVC, it also includes return type and calling convention.
 *        This function strips off everything before the last `::` (if any),
 *        and any parameter list or qualifiers after the name.
 */
inline std::string FuncName(const std::source_location& loc = std::source_location::current()) {
  std::string_view full = loc.function_name();
  // 1) find end of name (just before '(' or end of string)
  size_t paren = full.find('(');
  size_t name_end = (paren == std::string_view::npos ? full.size() : paren);

  // 2) try to strip namespaces/classes via '::'
  size_t colons = full.rfind("::", name_end);
  size_t start = (colons == std::string_view::npos ? 0 : colons + 2);

  // 3) on MSVC there's often a return-type + "__cdecl " prefix before the name
#ifdef _MSC_VER
  if (colons == std::string_view::npos) {
    // if no '::', drop everything up to last space before name_end
    size_t last_space = full.rfind(' ', name_end);
    if (last_space != std::string_view::npos) {
      start = last_space + 1;
    }
  }
#endif

  // 4) construct owning string of just the name
  return std::string{full.substr(start, name_end - start)};
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
