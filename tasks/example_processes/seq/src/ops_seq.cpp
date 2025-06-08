#include "example_processes/seq/include/ops_seq.hpp"

#include <cmath>
#include <numeric>
#include <vector>

#include "core/util/include/util.hpp"
#include "example_processes/common/include/common.hpp"

namespace nesterov_a_test_task_processes {

NesterovATestTaskSEQ::NesterovATestTaskSEQ(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskSEQ::ValidationImpl() { return (GetInput() > 0) && (GetOutput() == 0); }

bool NesterovATestTaskSEQ::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskSEQ::RunImpl() {
  auto input = GetInput();
  if (input == 0) {
    return false;
  }

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

  int counter = 0;
  for (int i = 0; i < num_threads; i++) {
    counter++;
  }

  GetOutput() /= counter;
  return GetOutput() > 0;
}

bool NesterovATestTaskSEQ::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace nesterov_a_test_task_processes
