#pragma once
#include <string>

/* NOLINTBEGIN */
#define INSTANTIATE_TEST_SUITE_P_NOLINT(prefix, test_case_name, generator) \
  INSTANTIATE_TEST_SUITE_P(prefix, test_case_name, generator)
/* NOLINTEND */

/* NOLINTBEGIN */
#define DEATH_TEST(test_suite_name, test_name) TEST(test_suite_name, test_name)
/* NOLINTEND */

namespace ppc::util {

std::string GetAbsolutePath(const std::string &relative_path);
int GetNumThreads();

}  // namespace ppc::util
