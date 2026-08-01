#include "example/threads/omp/include/ops_omp.hpp"

#include <atomic>
#include <numeric>
#include <vector>

#include "example/common/include/common.hpp"
#include "util/include/util.hpp"

namespace example_threads {

NesterovATestTaskOMP::NesterovATestTaskOMP(const InType &in) : BaseTask(in) {}

bool NesterovATestTaskOMP::ValidationImpl() {
  return (GetInput() > 0) && (GetMutableOutput() == 0);
}

bool NesterovATestTaskOMP::PreProcessingImpl() {
  GetMutableOutput() = 2 * GetInput();
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskOMP::RunImpl() {
  for (InType i = 0; i < GetInput(); i++) {
    for (InType j = 0; j < GetInput(); j++) {
      for (InType k = 0; k < GetInput(); k++) {
        std::vector<InType> tmp(i + j + k, 1);
        GetMutableOutput() += std::accumulate(tmp.begin(), tmp.end(), 0);
        GetMutableOutput() -= i + j + k;
      }
    }
  }

  const int num_threads = ppc::util::GetNumThreads();
  GetMutableOutput() *= num_threads;

  std::atomic<int> counter(0);
#pragma omp parallel default(none) shared(counter) num_threads(ppc::util::GetNumThreads())
  counter++;

  GetMutableOutput() /= counter;
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskOMP::PostProcessingImpl() {
  GetMutableOutput() -= GetInput();
  return GetMutableOutput() > 0;
}

}  // namespace example_threads
