#include "example/threads/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "example/common/include/common.hpp"
#include "util/include/util.hpp"

namespace example_threads {

NesterovATestTaskSEQ::NesterovATestTaskSEQ(const InType &in) : BaseTask(in) {}

bool NesterovATestTaskSEQ::ValidationImpl() {
  return (GetInput() > 0) && (GetMutableOutput() == 0);
}

bool NesterovATestTaskSEQ::PreProcessingImpl() {
  GetMutableOutput() = 2 * GetInput();
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskSEQ::RunImpl() {
  if (GetInput() == 0) {
    return false;
  }

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

  int counter = 0;
  for (int i = 0; i < num_threads; i++) {
    counter++;
  }

  if (counter != 0) {
    GetMutableOutput() /= counter;
  }
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskSEQ::PostProcessingImpl() {
  GetMutableOutput() -= GetInput();
  return GetMutableOutput() > 0;
}

}  // namespace example_threads
