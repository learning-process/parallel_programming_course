#pragma once
#include <string>
#include <string_view>

/* NOLINTBEGIN */
#define INSTANTIATE_TEST_SUITE_P_NOLINT(prefix, test_case_name, generator) \
  INSTANTIATE_TEST_SUITE_P(prefix, test_case_name, generator)
/* NOLINTEND */

/* NOLINTBEGIN */
#define DEATH_TEST(test_suite_name, test_name) TEST(test_suite_name, test_name)
/* NOLINTEND */

namespace ppc::util {

std::string GetAbsolutePath(const std::string &relative_path);
int GetPPCNumThreads();

template <typename T>
consteval std::string_view GetNamespace() {
#if defined(__clang__) || defined(__GNUC__)
  constexpr std::string_view func = __PRETTY_FUNCTION__;
  // example: "consteval std::string_view get_namespace() [with T = my_namespace::MyClass]"
  constexpr std::string_view key = "T = ";
#elif defined(_MSC_VER)
  constexpr std::string_view func = __FUNCSIG__;
  // example: "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl get_namespace<class
  // my_namespace::MyClass>(void)"
  constexpr std::string_view key = "get_namespace<";
#else
  static_assert(false, "Unsupported compiler");
#endif

  auto start = func.find(key);
  if (start == std::string_view::npos) return {};
  start += key.size();

  auto end = func.find("::", start);
  if (end == std::string_view::npos) return {};

  return func.substr(start, end - start);
}

}  // namespace ppc::util
