#include "util/include/osh_runtime.hpp"

#include <shmem.h>

#include <cstdlib>
#include <mutex>
#include <stdexcept>

namespace {

std::mutex g_runtime_mutex;
bool g_initialized = false;
bool g_finalized = false;
bool g_atexit_registered = false;

void FinalizeAtExit() noexcept {
  try {
    ppc::util::OshRuntime::Finalize();
  } catch (...) {
  }
}

void RegisterFinalizeAtExit() {
  if (!g_atexit_registered) {
    std::atexit(FinalizeAtExit);
    g_atexit_registered = true;
  }
}

}  // namespace

void ppc::util::OshRuntime::EnsureInitialized() {
  std::lock_guard lock(g_runtime_mutex);
  if (g_initialized) {
    return;
  }
  if (g_finalized) {
    throw std::runtime_error("OSH runtime was already finalized in this process");
  }

  shmem_init();
  g_initialized = true;
  RegisterFinalizeAtExit();
}

void ppc::util::OshRuntime::Finalize() {
  std::lock_guard lock(g_runtime_mutex);
  if (!g_initialized || g_finalized) {
    return;
  }

  shmem_finalize();
  g_initialized = false;
  g_finalized = true;
}

bool ppc::util::OshRuntime::IsInitialized() {
  std::lock_guard lock(g_runtime_mutex);
  return g_initialized;
}

int ppc::util::OshRuntime::MyPe() {
  EnsureInitialized();
  return shmem_my_pe();
}

int ppc::util::OshRuntime::NumPes() {
  EnsureInitialized();
  return shmem_n_pes();
}

void ppc::util::OshRuntime::BarrierAll() {
  EnsureInitialized();
  shmem_barrier_all();
}

void ppc::util::OshRuntime::GlobalExit(int status) {
  EnsureInitialized();
  shmem_global_exit(status);
}
