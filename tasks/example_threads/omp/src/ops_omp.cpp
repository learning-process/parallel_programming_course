#include "example_threads/omp/include/ops_omp.hpp"

#include <atomic>
#include <numeric>
#include <vector>

#include "example_threads/common/include/common.hpp"
#include "util/include/util.hpp"

namespace nesterov_a_test_task_threads {

NesterovATestTaskOMP::NesterovATestTaskOMP(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskOMP::ValidationImpl() {
  return (GetInput() > 0) && (GetOutput() == 0);
}

bool NesterovATestTaskOMP::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskOMP::RunImpl() {
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
#pragma omp parallel default(none) shared(counter) num_threads(ppc::util::GetNumThreads())
  counter++;

  GetOutput() /= counter;
  return GetOutput() > 0;
}

bool NesterovATestTaskOMP::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace nesterov_a_test_task_threads
