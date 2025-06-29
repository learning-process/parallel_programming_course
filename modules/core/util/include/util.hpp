#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <source_location>
#include <string>
#include <typeinfo>
#ifdef __GNUG__
#include <cxxabi.h>
#endif

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
std::string GetNamespace() {
  std::string name = typeid(T).name();
#ifdef __GNUG__
  int status = 0;
  std::unique_ptr<char, void (*)(void*)> demangled{abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status),
                                                   std::free};
  name = (status == 0) ? demangled.get() : name;
#endif
#if defined(_MSC_VER)
  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto& prefix : prefixes) {
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

inline std::shared_ptr<nlohmann::json> InitJSONPtr() { return std::make_shared<nlohmann::json>(); }

bool IsUnderMpirun();

}  // namespace ppc::util
