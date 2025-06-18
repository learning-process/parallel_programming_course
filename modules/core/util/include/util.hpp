#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4459)
#endif

#include <nlohmann/json.hpp>  // NOLINT(misc-include-cleaner)

/// @brief JSON namespace used for settings and config parsing.
using NlohmannJsonParseError = nlohmann::json::parse_error;  // NOLINT(misc-include-cleaner)
/// @brief JSON namespace used for settings and config typing.
using NlohmannJsonTypeError = nlohmann::json::type_error;  // NOLINT(misc-include-cleaner)
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define ASSERT_ANY_THROW_NOLINT(stmt) ASSERT_ANY_THROW(stmt)                                   // NOLINT
#define EXPECT_THROW_NOLINT(stmt, error) EXPECT_THROW(stmt, error)                             // NOLINT
#define TEST_NOLINT(test_suite_name, test_name) TEST(test_suite_name, test_name)               // NOLINT
#define ASSERT_DEATH_IF_SUPPORTED_NOLINT(stmt, name) ASSERT_DEATH_IF_SUPPORTED(stmt, name)     // NOLINT
#define TYPED_TEST_NOLINT(test_suite_name, test_name) TYPED_TEST(test_suite_name, test_name)   // NOLINT
#define INSTANTIATE_TEST_SUITE_P_WITH_NAME(n, t, g, ng) INSTANTIATE_TEST_SUITE_P(n, t, g, ng)  // NOLINT
#define INSTANTIATE_TEST_SUITE_P_NOLINT(n, t, g) INSTANTIATE_TEST_SUITE_P(n, t, g)             // NOLINT
// INSTANTIATE_TEST_SUITE_P | n, t, g, ng == name, test_case_name, generator, name_generator

namespace ppc::util {

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

// NOLINTNEXTLINE(misc-include-cleaner)
inline std::shared_ptr<nlohmann::json> InitJSONPtr() { return std::make_shared<nlohmann::json>(); }

bool IsUnderMpirun();

}  // namespace ppc::util
