#include <cstdlib>
#include <stdexcept>

#include "util/include/osh_runtime.hpp"

void ppc::util::OshRuntime::EnsureInitialized() {
  throw std::runtime_error("OSH runtime is not available on this platform");
}

void ppc::util::OshRuntime::Finalize() {}

bool ppc::util::OshRuntime::IsInitialized() {
  return false;
}

int ppc::util::OshRuntime::MyPe() {
  throw std::runtime_error("OSH runtime is not available on this platform");
}

int ppc::util::OshRuntime::NumPes() {
  throw std::runtime_error("OSH runtime is not available on this platform");
}

void ppc::util::OshRuntime::BarrierAll() {
  throw std::runtime_error("OSH runtime is not available on this platform");
}

void ppc::util::OshRuntime::GlobalExit(int status) {
  std::exit(status);
}
