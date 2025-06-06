#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include "omp.h"

#include "core/task/func_tests/test_task.hpp"
#include "core/util/include/util.hpp"

TEST(ppc_func_tests_common, threads_control_check_openmp) {
  int ppc_num_threads = ppc::util::GetNumThreads();

  int omp_num_threads = -1;
#pragma omp parallel
  {
      omp_num_threads = omp_get_num_threads();
  };

  // Check Result
  ASSERT_EQ(ppc_num_threads, omp_num_threads);
}