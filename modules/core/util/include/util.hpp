#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4459)  // переменная 'last' перекрывает глобальную
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

#if (defined(__clang__) || defined(__GNUC__)) && !defined(_MSC_VER)
template <typename T>
consteval std::string_view GetNamespace() {
  constexpr std::string_view kFunc = __PRETTY_FUNCTION__;
  // example: "consteval std::string_view get_namespace() [with T = my_namespace::MyClass]"
  constexpr std::string_view kKey = "T = ";
#elif defined(_MSC_VER)
template <typename T>
constexpr std::string_view GetNamespace() {
  constexpr std::string_view kFunc = __FUNCSIG__;
  // example: "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl get_namespace<class
  // my_namespace::MyClass>(void)"
  constexpr std::string_view kKey = "get_namespace<";
#else
static_assert(false, "Unsupported compiler");
#endif

  auto start = kFunc.find(kKey);
  if (start == std::string_view::npos) {
    return {};
  }
  start += kKey.size();

  auto end = kFunc.find("::", start);
  if (end == std::string_view::npos) {
    return {};
  }

  return kFunc.substr(start, end - start);
}

}  // namespace ppc::util
