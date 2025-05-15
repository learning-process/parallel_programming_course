#pragma once
#include <string>

// NOLINTNEXTLINE
#define INSTANTIATE_TEST_SUITE_P_NOLINT(prefix, test_case_name, generator) \
  INSTANTIATE_TEST_SUITE_P(prefix, test_case_name, generator)  // NOLINT

namespace ppc::util {

std::string GetAbsolutePath(const std::string &relative_path);
int GetPPCNumThreads();

}  // namespace ppc::util
