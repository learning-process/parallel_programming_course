#include "example/threads/stl/include/ops_stl.hpp"

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include "example/common/include/common.hpp"
#include "util/include/util.hpp"

namespace example_threads {

NesterovATestTaskSTL::NesterovATestTaskSTL(const InType &in) : BaseTask(in) {}

bool NesterovATestTaskSTL::ValidationImpl() {
  return (GetInput() > 0) && (GetMutableOutput() == 0);
}

bool NesterovATestTaskSTL::PreProcessingImpl() {
  GetMutableOutput() = 2 * GetInput();
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskSTL::RunImpl() {
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
  std::vector<std::thread> threads(num_threads);
  GetMutableOutput() *= num_threads;

  std::atomic<int> counter(0);
  for (std::thread &thread : threads) {
    thread = std::thread([&counter]() -> void { counter++; });
    thread.join();
  }

  GetMutableOutput() /= counter;
  return GetMutableOutput() > 0;
}

bool NesterovATestTaskSTL::PostProcessingImpl() {
  GetMutableOutput() -= GetInput();
  return GetMutableOutput() > 0;
}

}  // namespace example_threads
