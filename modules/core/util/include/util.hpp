#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4459)
#endif

#include <nlohmann/json.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* NOLINTBEGIN */
#define INSTANTIATE_TEST_SUITE_P_NOLINT(prefix, test_case_name, generator, custom_test_name) \
  INSTANTIATE_TEST_SUITE_P(prefix, test_case_name, generator, custom_test_name)
/* NOLINTEND */

/* NOLINTBEGIN */
#define DEATH_TEST(test_suite_name, test_name) TEST(test_suite_name, test_name)
/* NOLINTEND */

namespace ppc::util {

enum GTestParamIndex : uint8_t { kTaskGetter, kNameTest, kTestParams };

std::string GetAbsolutePath(const std::string &relative_path);
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

bool IsUnderMpirun();

}  // namespace ppc::util
