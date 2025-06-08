#include "core/util/include/util.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "omp.h"

namespace my::nested {
struct Type {};
}  // namespace my::nested

TEST(util_tests, extracts_correct_namespace) {
  constexpr std::string_view kNs = ppc::util::GetNamespace<my::nested::Type>();
  EXPECT_EQ(kNs, "my::nested");
}

TEST(util_tests, threads_control_check_openmp_disabled_valgrind) {
  int ppc_num_threads = ppc::util::GetNumThreads();

  int omp_num_threads = -1;
#pragma omp parallel default(none) shared(omp_num_threads) num_threads(ppc::util::GetNumThreads())
  omp_num_threads = omp_get_num_threads();

  // Check Result
  ASSERT_EQ(ppc_num_threads, omp_num_threads);
}
