#include <gtest/gtest.h>
#include <tbb/global_control.h>

int main(int argc, char** argv) {
#ifdef _WIN32
  size_t len;
  char omp_env[100];
  errno_t err = getenv_s(&len, omp_env, sizeof(omp_env), "OMP_NUM_THREADS");
  if (err != 0 || len == 0) {
    omp_env[0] = '\0';
  }
#else
  const char* omp_env = std::getenv("OMP_NUM_THREADS");
#endif
  int num_threads = (omp_env != nullptr) ? std::atoi(omp_env) : 1;

  // Limit the number of threads in TBB
  tbb::global_control control(tbb::global_control::max_allowed_parallelism, num_threads);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
