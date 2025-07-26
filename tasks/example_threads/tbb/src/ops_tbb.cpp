#include "example_threads/tbb/include/ops_tbb.hpp"

#include <tbb/tbb.h>

#include <atomic>
#include <numeric>
#include <util/include/util.hpp>
#include <vector>

#include "example_threads/common/include/common.hpp"
#include "oneapi/tbb/parallel_for.h"

namespace nesterov_a_test_task_threads {

NesterovATestTaskTBB::NesterovATestTaskTBB(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskTBB::ValidationImpl() {
  return (GetInput() > 0) && (GetOutput() == 0);
}

bool NesterovATestTaskTBB::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskTBB::RunImpl() {
  for (InType i = 0; i < GetInput(); i++) {
    for (InType j = 0; j < GetInput(); j++) {
      for (InType k = 0; k < GetInput(); k++) {
        std::vector<InType> tmp(i + j + k, 1);
        GetOutput() += std::accumulate(tmp.begin(), tmp.end(), 0);
        GetOutput() -= i + j + k;
      }
    }
  }

  const int num_threads = ppc::util::GetNumThreads();
  GetOutput() *= num_threads;

  std::atomic<int> counter(0);
  tbb::parallel_for(0, ppc::util::GetNumThreads(), [&](int /*i*/) { counter++; });

  GetOutput() /= counter;
  return GetOutput() > 0;
}

bool NesterovATestTaskTBB::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace nesterov_a_test_task_threads
