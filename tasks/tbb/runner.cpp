#include <gtest/gtest.h>
#include <tbb/global_control.h>

#include "core/util/util.hpp"

int main(int argc, char** argv) {
  int num_threads = ppc::util::GetPPCNumThreads();

  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, num_threads);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
