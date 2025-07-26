#include "example_threads/stl/include/ops_stl.hpp"

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include "example_threads/common/include/common.hpp"
#include "util/include/util.hpp"

namespace nesterov_a_test_task_threads {

NesterovATestTaskSTL::NesterovATestTaskSTL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool NesterovATestTaskSTL::ValidationImpl() {
  return (GetInput() > 0) && (GetOutput() == 0);
}

bool NesterovATestTaskSTL::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool NesterovATestTaskSTL::RunImpl() {
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
  std::vector<std::thread> threads(num_threads);
  GetOutput() *= num_threads;

  std::atomic<int> counter(0);
  for (int i = 0; i < num_threads; i++) {
    threads[i] = std::thread([&]() { counter++; });
    threads[i].join();
  }

  GetOutput() /= counter;
  return GetOutput() > 0;
}

bool NesterovATestTaskSTL::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace nesterov_a_test_task_threads
