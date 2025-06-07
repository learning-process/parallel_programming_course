#include "core/util/include/util.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/task/func_tests/test_task.hpp"
#include "omp.h"

TEST(ppc_func_tests_common, threads_control_check_openmp) {
  int ppc_num_threads = ppc::util::GetNumThreads();

  int omp_num_threads = -1;
#pragma omp parallel default(none) shared(omp_num_threads) num_threads(ppc::util::GetNumThreads())
  omp_num_threads = omp_get_num_threads();

  // Check Result
  ASSERT_EQ(ppc_num_threads, omp_num_threads);
}
