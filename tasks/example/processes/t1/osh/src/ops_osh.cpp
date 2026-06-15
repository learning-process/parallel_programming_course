#include "example/processes/t1/osh/include/ops_osh.hpp"

#include <shmem.h>

#include <numeric>
#include <vector>

#include "example/common/include/common.hpp"
#include "util/include/osh_runtime.hpp"
#include "util/include/util.hpp"

namespace example_processes_t1 {

NesterovATestTaskOSH::NesterovATestTaskOSH(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskOSH::ValidationImpl() {
  return (GetInput() > 0) && (GetOutput() == 0);
}

bool NesterovATestTaskOSH::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskOSH::RunImpl() {
  ppc::util::OshRuntime::EnsureInitialized();

  const int my_pe = shmem_my_pe();
  const int num_pes = shmem_n_pes();
  if (GetInput() == 0 || my_pe < 0 || num_pes <= 0) {
    return false;
  }

  for (InType i = my_pe; i < GetInput(); i += num_pes) {
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

  if (counter != 0) {
    GetOutput() /= counter;
  }

  shmem_barrier_all();
  return GetOutput() > 0;
}

bool NesterovATestTaskOSH::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace example_processes_t1
